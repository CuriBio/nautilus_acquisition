/******************************************************************************/
/* Copyright (C) Teledyne Photometrics. All rights reserved.                  */
/******************************************************************************/
/** @file */
#pragma once
#ifndef PVCAM_HELPER_COLOR_H
#define PVCAM_HELPER_COLOR_H

/* PVCAM */
#include <master.h>
#include <pvcam.h>

/**
Major version number.

This part of version number changes whenever a binary incompatible modification
to library API is done, e.g. changed name or prototype of existing function.

Applications can check this at compile time to use appropriate syntax.
*/
#define PH_COLOR_VERSION_MAJOR 2
/**
Minor version number.

This part of version number changes whenever a binary compatible modification to
library API is done, e.g. added new function or data type.

Applications can check this at compile time to use new functions and/or types.
*/
#define PH_COLOR_VERSION_MINOR 1

/** Maximum length of the error message including null terminator. */
#define PH_COLOR_MAX_ERROR_LEN 4096

/** Error codes returned by functions. */
enum PH_COLOR_ERROR
{
    /** @showinitializer */
    PH_COLOR_ERROR_NONE = 0,
    /** @showinitializer */
    PH_COLOR_ERROR_GENERIC = 1,
    /** @showinitializer */
    PH_COLOR_ERROR_ARGUMENT = 2,
    /** @showinitializer */
    PH_COLOR_ERROR_ARGUMENT_NULL = 3,
    /** @showinitializer */
    PH_COLOR_ERROR_ARGUMENT_OUT_OF_RANGE = 4,
    /** @showinitializer */
    PH_COLOR_ERROR_NOT_SUPPORTED = 11,
    /** @showinitializer */
    PH_COLOR_ERROR_BUFFER_TOO_SMALL = 100,
    /** @showinitializer */
    PH_COLOR_ERROR_OP_FAILED = 101,
    /** @showinitializer */
    PH_COLOR_ERROR_OP_IN_PROGRESS = 102,
    /** @showinitializer */
    PH_COLOR_ERROR_OP_ABORTED = 103,
    /** The offset added to int16 error code from PVCAM. */
    /** @showinitializer */
    PH_COLOR_ERROR_PVCAM_BASE = 0x7F0000 /* + PVCAM-error */
};
typedef enum PH_COLOR_ERROR PH_COLOR_ERROR;

/** Debayer algorithms. */
enum PH_COLOR_DEBAYER_ALG
{
    /** @showinitializer */
    PH_COLOR_DEBAYER_ALG_NEAREST = 0,   /**< Nearest neighbor */
    PH_COLOR_DEBAYER_ALG_BILINEAR      /**< Bilinear averaging */
    // TODO: Add bicubic algorithm back
    //PH_COLOR_DEBAYER_ALG_BICUBIC        /**< Bicubic */
};
typedef enum PH_COLOR_DEBAYER_ALG PH_COLOR_DEBAYER_ALG;

/** Auto-exposure algorithms. */
enum PH_COLOR_AUTOEXP_ALG
{
    /** @showinitializer */
    /**
    Image averaging

    Algorithm selects exposure which gives image, where the average brightness
    calculated from all pixels is nearing the photographic middle gray.
    */
    PH_COLOR_AUTOEXP_ALG_AVERAGE = 0,
    /**
    Histogram based algorithm

    Generates histogram for each channel of the image. Sets exposure
    so only 1% of the brightest pixels are saturated.
    */
    PH_COLOR_AUTOEXP_ALG_HISTOGRAM
};
typedef enum PH_COLOR_AUTOEXP_ALG PH_COLOR_AUTOEXP_ALG;

/**
RGB and Debayer output buffer formats.

The endianness of the data will be consistent with the processor used.
For x86 and x64 architecture it is Little Endian.
*/
enum PH_COLOR_RGB_FORMAT
{
    /* RGB formats from 16bit raw sensor */
    PH_COLOR_RGB_FORMAT_RGB48,      /**< 48bpp interleaved
                                        (normal for 16bit sensors) */
    PH_COLOR_RGB_FORMAT_PLANE16,    /**< 48bpp planar, channels in order RGB */

    /* RGB formats from 8bit raw sensor */
    PH_COLOR_RGB_FORMAT_RGB24,      /**< 24bpp RGB (normal for 8bit sensors) */
    PH_COLOR_RGB_FORMAT_BGR24,      /**< 24bpp BGR, common Windows format */
    PH_COLOR_RGB_FORMAT_PLANE8,     /**< 24bpp planar, channels in order RGB */
    PH_COLOR_RGB_FORMAT_ARGB32,     /**< 32bpp aRGB with alpha channel,
                                          format of Mac pixel-map */
    PH_COLOR_RGB_FORMAT_BGRA32,     /**< 32bpp BGRa with alpha channel,
                                          common Windows format */
    PH_COLOR_RGB_FORMAT_RGBA32      /**< 32bpp RGBa with alpha channel */
};
typedef enum PH_COLOR_RGB_FORMAT PH_COLOR_RGB_FORMAT;

/**
The context holding camera specific state.

The context allows application to access currently used settings.
If the application changes any value, the library has to be notified by passing
the context to #ph_color_context_apply_changes function. Without calling that
function all other functions will operate on outdated internal copy stored in
@p p.

This approach for instance allows speed up of white balance algorithm with help
of lookup tables.
*/
struct ph_color_context
{
    int32 algorithm;    /**< Debayer algorithm select from enum
                            #PH_COLOR_DEBAYER_ALG. */
    int32 pattern;      /**< Sensor bayer pattern from PVCAM PL_COLOR_MODES enum
                            as reported by the camera. */
    uns16 bitDepth;     /**< Image bit depth from the camera speed table. */
    int32 rgbFormat;    /**< RGB buffer data format from
                            #PH_COLOR_RGB_FORMAT. */

    flt32 redScale;     /**< Red channel scale value for the white balance
                            function #ph_color_white_balance.
                            The scale must be zero or positive value. */
    flt32 greenScale;   /**< Green channel scale value for the white balance
                            function #ph_color_white_balance.
                            The scale must be zero or positive value. */
    flt32 blueScale;    /**< Blue channel scale value for the white balance
                            function #ph_color_white_balance.
                            The scale must be zero or positive value. */

    void* p;            /**< Private context data for internal use. */

    int32 autoExpAlgorithm; /**< Auto exposure algorithm select from enum
                                  #PH_COLOR_AUTOEXP_ALG. */

    rs_bool forceCpu;   /**< If set to FALSE (or zero) an auto-detection is done
                            and functions use GPU with sufficient compute
                            capability. Otherwise processing falls back to CPU.
                            A TRUE (or non-zero) value forces processing on CPU
                            only ignoring all GPUs. */

    uns16 sensorWidth;  /**< Max. width of image buffer to be allowed in all
                            other functions.
                            If @p forceCpu is set this value is ignored. */
    uns16 sensorHeight; /**< Max. height of image buffer to be allowed in all
                            other functions.
                            If @p forceCpu is set this value is ignored. */

    uns16 alphaValue;   /**< The value of alpha channel for each pixel in output
                            buffer after debayering or conversion from format
                            without alpha channel to format with alpha channel.
                            The zero value means fully transparent image.
                            For fully opaque image this value is internally
                            limited by @p bitDepth to (2^bitDepth - 1). */
};
typedef struct ph_color_context ph_color_context;

#ifdef PV_C_PLUS_PLUS
extern "C"
{
#endif

/******************************************************************************/
/**
Offers all three parts of the helper library version.

This function can be called at any time even before the color library has been
initialized. It could fail only in case the NULL pointer is given as parameter.

@param[out] pVerMajor   Major version number, returns at runtime the same as
                        macro #PH_COLOR_VERSION_MAJOR.
@param[out] pVerMinor   Minor version number, returns at runtime the same as
                        macro #PH_COLOR_VERSION_MINOR.
@param[out] pVerBuild   Build version number, provided for completeness.
                        It is always increasing number changed with every
                        modification in library.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_get_lib_version(
        uns16* pVerMajor, uns16* pVerMinor, uns16* pVerBuild);
/** For loading #ph_color_get_lib_version at run-time. */
typedef int32 (PV_DECL *ph_color_get_lib_version_fn)(
        uns16* pVerMajor, uns16* pVerMinor, uns16* pVerBuild);
/** For loading #ph_color_get_lib_version at run-time. */
#define ph_color_get_lib_version_fn_name "ph_color_get_lib_version"

/******************************************************************************/
/**
Provides human-readable message describing last error.

After any function returns an error condition, #ph_color_get_last_error_message
can be called on the same thread to retrieve a more detailed, human-readable
error description string. The caller provides a buffer @p pBuffer to receive the
string of max. length @p pBufferSize bytes.

If the error message exceeds @p pBufferSize bytes, then error
#PH_COLOR_ERROR_BUFFER_TOO_SMALL is returned and @p pBufferSize value is
replaced with the minimum necessary buffer size. A second call can then be made
with a buffer of the necessary size to retrieve the message.

If the buffer is too small, then the message is copied to the length available
even if the #PH_COLOR_ERROR_BUFFER_TOO_SMALL error code is returned.

A @p pBufferSize of zero is allowed in order to query the required buffer size
for a subsequent call.

Maximum message size is limited by #PH_COLOR_MAX_ERROR_LEN value.

@param[out]    pBuffer      Output buffer filled with last error message.
@param[in,out] pBufferSize  Size of given buffer @a pBuffer in bytes.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_get_last_error_message(
        char* pBuffer, uns32* pBufferSize);
/** For loading #ph_color_get_last_error_message at run-time. */
typedef int32 (PV_DECL *ph_color_get_last_error_message_fn)(
        char* pBuffer, uns32* pBufferSize);
/** For loading #ph_color_get_last_error_message at run-time. */
#define ph_color_get_last_error_message_fn_name \
        "ph_color_get_last_error_message"

/******************************************************************************/
/**
Provides error message for all known error codes.

This function returns generic error message based on given error code. It never
fails and can be called at any time, event before #ph_color_context_create.

Instead, it is recommended to call #ph_color_get_last_error_message right after
an error occurred to get more detailed error message.

The message length returned by this function is not limited to
#PH_COLOR_MAX_ERROR_LEN, but it is usually very short.

@param[in]  errorCode    An error code - value from #PH_COLOR_ERROR enum.

@return Returns generic message for given error code.
*/
const char* PV_DECL ph_color_get_error_message(
        int32 errorCode);
/** For loading ph_color_get_error_message at run-time. */
typedef const char* (PV_DECL *ph_color_get_error_message_fn)(
        int32 errorCode);
/** For loading ph_color_get_error_message at run-time. */
#define ph_color_get_error_message_fn_name "ph_color_get_error_message"

/******************************************************************************/
/**
Allocate and initialize the context used in the color helper library.

New context must be created for every camera to allow multiple cameras to be
used simultaneously.

The @p pContext value is ignored and replaced by new context. Be sure to release
previous context before calling this function to not have memory leaks.

Default context values are:
- @p algorithm = #PH_COLOR_DEBAYER_ALG_BILINEAR
- @p pattern = COLOR_RGGB
- @p bitDepth = 16
- @p rgbFormat = #PH_COLOR_RGB_FORMAT_RGB48
- @p redScale = 1.0
- @p greenScale = 1.0
- @p blueScale = 1.0
- @p autoExpAlgorithm = PH_COLOR_AUTOEXP_ALG_AVERAGE
- @p forceCpu = FALSE
- @p sensorWidth = 0
- @p sensorHeight = 0
- @p alphaValue = 0xFFFF

The sensor size is used to allocate internal CUDA device buffer used for
processing on GPU, but only if @p forceCpu is FALSE and auto-detection finds
usable GPU.

Because the default sensor size is set to 0x0 such context cannot be immediately
used for GPU processing. Application has to override it either with real sensor
size or the max. size of an image that will be passed to functions.

@param[out] pContext    A location where a pointer to newly allocated context
                        will be stored.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_context_create(
        ph_color_context** pContext);
/** For loading #ph_color_context_create at run-time. */
typedef int32 (PV_DECL *ph_color_context_create_fn)(
        ph_color_context** pContext);
/** For loading #ph_color_context_create at run-time. */
#define ph_color_context_create_fn_name "ph_color_context_create"

/******************************************************************************/
/**
Release the context previously created by #ph_color_context_create function.

This function will release memory allocated for context and sets @p pContext to
null.

@param[in,out] pContext     A location where a pointer to already allocated
                            context is stored.
*/
void PV_DECL ph_color_context_release(
        ph_color_context** pContext);
/** For loading #ph_color_context_release at run-time. */
typedef void (PV_DECL *ph_color_context_release_fn)(
        ph_color_context** pContext);
/** For loading #ph_color_context_release at run-time. */
#define ph_color_context_release_fn_name "ph_color_context_release"

/******************************************************************************/
/**
Apply changes being done in context structure.

This function will compare context with internal one and will update all
internal data (e.g. lookup tables) stored in context's private member.

All other functions work with internal data and ignore any changes on context
structure done by application. Only this function makes new settings available
for future processing.

@note This function will allow only setting 8 bit RGB format for bit depths from
1 to 8 and 16 bit output format for bit depths from 9 to 16.
See #PH_COLOR_RGB_FORMAT for supported output formats.

@param[in] pContext     A context with updated settings to be applied.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_context_apply_changes(
        const ph_color_context* pContext);
/** For loading #ph_color_context_apply_changes at run-time. */
typedef int32 (PV_DECL *ph_color_context_apply_changes_fn)(
        const ph_color_context* pContext);
/** For loading #ph_color_context_apply_changes at run-time. */
#define ph_color_context_apply_changes_fn_name "ph_color_context_apply_changes"

/******************************************************************************/
/**
Debayer a raw image in the input buffer to a RGB image in the output buffer.

The size of the output buffer must be three times larger than the input buffer
(in bytes) for the RGB data, or four times larger for formats with alpha
channel.

@note This function will only convert 8 bit raw images to 8 bit output formats
and 16 bit raw images to 16 bit output formats. This is enforced by function
#ph_color_context_apply_changes because all functions work with internal copy
of context which is always valid (it changes only when successfully applied).

@param[in]  pContext    A context with current settings.
@param[in]  pInBufRaw   Pointer to raw image from camera.
@param[in]  roi         ROI relative to full frame camera sensor size.
@param[out] pOutBufRgb  Pointer to output buffer being large enough to hold
                        debayered image in output format as set in @p pContext,
                        e.g. 48bpp RGB image should be 3 times larger than input
                        buffer.
                        If output buffer contains alpha channel its value is set
                        to @c pContext->alphaValue (limited by max. bitDepth
                        value).

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_debayer(
        const ph_color_context* pContext,
        const void* pInBufRaw, rgn_type roi, void* pOutBufRgb);
/** For loading #ph_color_debayer at run-time. */
typedef int32 (PV_DECL *ph_color_debayer_fn)(
        const ph_color_context* pContext,
        const void* pInBufRaw, rgn_type roi, void* pOutBufRgb);
/** For loading #ph_color_debayer at run-time. */
#define ph_color_debayer_fn_name "ph_color_debayer"

/******************************************************************************/
/**
White balance the RGB image in place using the currently set RGB scale values.

This function is normally called after a call to #ph_color_context_apply_changes
which will set the RGB scale values and then for every new every frame.

Currently used RGB scale values can be accessed via context.

@param[in]      pContext    A context with current settings.
@param[in,out]  pBufRgb     RGB image buffer to white balance.
@param[in]      roi         ROI relative to full frame camera sensor size.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_white_balance(
        const ph_color_context* pContext, void* pBufRgb, rgn_type roi);
/** For loading #ph_color_white_balance at run-time. */
typedef int32 (PV_DECL *ph_color_white_balance_fn)(
        const ph_color_context* pContext, void* pBufRgb, rgn_type roi);
/** For loading #ph_color_white_balance at run-time. */
#define ph_color_white_balance_fn_name "ph_color_white_balance"

/******************************************************************************/
/**
Wrapper for #ph_color_debayer and #ph_color_white_balance functions.

The size of the output buffer must be three times larger than the input buffer
(in bytes) for the RGB data.

@note This function will only convert 8 bit raw images to 8 bit output formats
and 16 bit raw images to 16 bit output formats. This is enforced by function
#ph_color_context_apply_changes because all functions work with internal copy
of context which is always valid (it changes only when successfully applied).

White balance is done in place on @p pOutBufRgb buffer using the currently set
RGB scale values which are stored in given context.

@param[in]  pContext    A context with current settings.
@param[in]  pInBufRaw   Pointer to raw image from camera.
@param[in]  roi         ROI relative to full frame camera sensor size.
@param[out] pOutBufRgb  Pointer to output buffer being large enough to hold
                        debayered image in output format as set in pContext,
                        e.g. 48bpp RGB image should be 3 times larger than input
                        buffer.
                        If output buffer contains alpha channel its value is set
                        to @c pContext->alphaValue (limited by max. bitDepth
                        value).
                        This buffer is also white-balanced.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_debayer_and_white_balance(
        const ph_color_context* pContext,
        const void* pInBufRaw, rgn_type roi, void* pOutBufRgb);
/** For loading #ph_color_debayer_and_white_balance at run-time. */
typedef int32 (PV_DECL *ph_color_debayer_and_white_balance_fn)(
        const ph_color_context* pContext,
        const void* pInBufRaw, rgn_type roi, void* pOutBufRgb);
/** For loading #ph_color_debayer_and_white_balance at run-time. */
#define ph_color_debayer_and_white_balance_fn_name \
        "ph_color_debayer_and_white_balance"

/******************************************************************************/
/**
For the camera specified search optimal exposure time.

The white/gray area ROI is specified as coordinates relative to the sensor full
frame size.

When this function returns successfully, the optimal exposure time is stored in
argument @p pExpTime.

This function uses the current camera settings for speed, gain state, clearing
mode and exposure resolution. It acquires frames using polling and does not
set up any callbacks with PVCAM.

@attention This function uses blocking calls to PVCAM core to acquire images to
    measure the exposure level. Do not call this function while any PVCAM
    functions are in progress or any callbacks have been registered.

@param[in]  pContext    A context with current settings.
@param[in]  hcam        Handle to open camera to measure exposure.
@param[in]  roi         ROI of white/gray area relative to full frame.
@param[out] pBufRaw     A buffer for raw image big enough for @p roi.
@param[out] pBufRgb     A buffer for debayered RGB image big enough for @p roi.
@param[out] pExpTime    Optimal exposure time.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_auto_exposure(
        const ph_color_context* pContext, int16 hcam, rgn_type roi,
        void* pBufRaw, void* pBufRgb, uns32* pExpTime);
/** For loading #ph_color_auto_exposure at run-time. */
typedef int32 (PV_DECL *ph_color_auto_exposure_fn)(
        const ph_color_context* pContext, int16 hcam, rgn_type roi,
        void* pBufRaw, void* pBufRgb, uns32* pExpTime);
/** For loading #ph_color_auto_exposure at run-time. */
#define ph_color_auto_exposure_fn_name "ph_color_auto_exposure"

/******************************************************************************/
/**
Interrupt the blocking #ph_color_auto_exposure function.

This function can be called multiple times as well as if there is not auto
exposure in progress. In both cases function returns without error.

As the auto exposure is blocking the abort has to be issued from another thread.

Because of #ph_color_auto_exposure function uses polling every a few
milliseconds, this function does not need a camera handle argument. It only sets
an abort flag and exits immediately. The abort flag is periodically tested in
polling loop.

@param[in]  pContext    A context with current settings.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_auto_exposure_abort(
        const ph_color_context* pContext);
/** For loading #ph_color_auto_exposure_abort at run-time. */
typedef int32 (PV_DECL *ph_color_auto_exposure_abort_fn)(
        const ph_color_context* pContext);
/** For loading #ph_color_auto_exposure_abort at run-time. */
#define ph_color_auto_exposure_abort_fn_name "ph_color_auto_exposure_abort"

/******************************************************************************/
/**
Calculate RGB scale factors from given debayered image.

The white/gray area ROI is specified as coordinates relative to the sensor full
frame size.

The @p pBufRgb buffer should contain debayered image captured with optimal
exposure time. That time can be received e.g. from #ph_color_auto_exposure
function.

When this function returns successfully, the RGB scale factors are stored in
arguments @p pRedScale, @p pGreenScale and @p pBlueScale respectively.

@param[in]  pContext    A context with current settings.
@param[in]  roi         ROI of white/gray area relative to full frame.
@param[in]  pBufRgb     A buffer with debayered RGB image big enough for @p roi.
@param[out] pRedScale   Scale factor for red channel.
@param[out] pGreenScale Scale factor for green channel.
@param[out] pBlueScale  Scale factor for blue channel.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_auto_white_balance(
        const ph_color_context* pContext, rgn_type roi, const void* pBufRgb,
        flt32* pRedScale, flt32* pGreenScale, flt32* pBlueScale);
/** For loading #ph_color_auto_white_balance at run-time. */
typedef int32 (PV_DECL *ph_color_auto_white_balance_fn)(
        const ph_color_context* pContext, rgn_type roi, const void* pBufRgb,
        flt32* pRedScale, flt32* pGreenScale, flt32* pBlueScale);
/** For loading #ph_color_auto_white_balance at run-time. */
#define ph_color_auto_white_balance_fn_name "ph_color_auto_white_balance"

/******************************************************************************/
/**
Wrapper for #ph_color_auto_exposure and #ph_color_auto_white_balance functions.

This function internally allocates buffers for raw and RGB images.

Any of the four output arguments (@p pExpTime, @p pRedScale, @p pGreenScale and
@p pBlueScale) can be null if the value is not important.

@param[in]  pContext    A context with current settings.
@param[in]  hcam        Handle to open camera to measure exposure.
@param[in]  roi         ROI of white/gray area relative to full frame.
@param[out] pExpTime    Optimal exposure time.
@param[out] pRedScale   Scale factor for red channel.
@param[out] pGreenScale Scale factor for green channel.
@param[out] pBlueScale  Scale factor for blue channel.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_auto_exposure_and_white_balance(
        const ph_color_context* pContext, int16 hcam, rgn_type roi,
        uns32* pExpTime, flt32* pRedScale, flt32* pGreenScale,
        flt32* pBlueScale);
/** For loading #ph_color_auto_exposure_and_white_balance at run-time. */
typedef int32 (PV_DECL *ph_color_auto_exposure_and_white_balance_fn)(
        const ph_color_context* pContext, int16 hcam, rgn_type roi,
        uns32* pExpTime, flt32* pRedScale, flt32* pGreenScale,
        flt32* pBlueScale);
/** For loading #ph_color_auto_exposure_and_white_balance at run-time. */
#define ph_color_auto_exposure_and_white_balance_fn_name \
        "ph_color_auto_exposure_and_white_balance"

/******************************************************************************/
/**
Convert image format from one RGB format and/or bit depth to another.

This function can be used to up-scale or down-scale the pixel value based on bit
depth parameters @p inBitDepth and @p outBitDepth. It can also re-order RGB
parts of each pixel so for instance the buffer could be directly passed to
libtiff library to save image as TIFF or to wxImage::SetData to display picture
on the screen.

If output buffer contains alpha channel its value is either converted input
buffer alpha channel or @c pContext->alphaValue value (limited by max.
@p toBitDepth value) is used in case input buffer does not have alpha channel.
In case of upscaling to higher bit depth the alpha value is first limited by
@p fromBitDepth and then upscaled to @p toBitdepth by bit-shift.

Given context @p pContext internally stores e.g. GPU device buffers that can be
utilized by this function in one of next releases.

@param[in]  pContext        A context with current settings.
@param[in]  roi             Converted region relative to camera sensor size.
@param[in]  pFromBufRgb     Input buffer with debayered image.
@param[in]  fromRgbFormat   RGB format of the input buffer.
@param[in]  fromBitDepth    Pixel bit depth of the input buffer.
@param[out] pToBufRgb       Output buffer with debayered image.
@param[in]  toRgbFormat     RGB format of the output buffer.
@param[in]  toBitDepth      Pixel bit depth of the output buffer.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_convert_format(
        const ph_color_context* pContext, rgn_type roi,
        const void* pFromBufRgb, int32 fromRgbFormat, uns16 fromBitDepth,
        void* pToBufRgb, int32 toRgbFormat, uns16 toBitDepth);
/** For loading #ph_color_convert_format at run-time. */
typedef int32 (PV_DECL *ph_color_convert_format_fn)(
        const ph_color_context* pContext, rgn_type roi,
        const void* pFromBufRgb, int32 fromRgbFormat, uns16 fromBitDepth,
        void* pToBufRgb, int32 toRgbFormat, uns16 toBitDepth);
/** For loading #ph_color_convert_format at run-time. */
#define ph_color_convert_format_fn_name "ph_color_convert_format"

/******************************************************************************/
/**
Allocate an image buffer to be passed to other functions in this library.

Since color helper library can speed up and execute operations on GPU there are
some requirements from CUDA side on used buffers to make it possible.
The buffer is then considered as pinned memory by CUDA.

If the highest performance is important for the application, all buffers passed
to color library functions have to be allocated with #ph_color_buffer_alloc
function.

Otherwise any kind of buffer can be used but there will be done extra copy
operations by CUDA that significantly slows down whole image processing.

The @p pBuffer value is ignored and replaced by new one. Be sure to free
previous buffer before calling this function to not have memory leaks.

@note From CUDA documentation: Allocating excessive amounts of pinned memory may
    degrade system performance, since it reduces the amount of memory available
    to the system for paging.

@note The buffer is allocated as pinned memory if the library detects usable GPU
    even in case the @p forceCpu member in context structure forces processing
    on CPU only. It is application's responsibility to balance GPU acceleration
    using pinned buffer and system performance by not overusing pinned buffers.

@param[out] pBuffer     A location where a pointer to newly allocated buffer
                        will be stored.
@param[in]  bufferSize  Size of the buffer to allocate in bytes.

@return Returns error code from #PH_COLOR_ERROR enumeration.
*/
int32 PV_DECL ph_color_buffer_alloc(
        void** pBuffer, uns32 bufferSize);
/** For loading #ph_color_buffer_alloc at run-time. */
typedef int32 (PV_DECL *ph_color_buffer_alloc_fn)(
        void** pBuffer, uns32 bufferSize);
/** For loading #ph_color_buffer_alloc at run-time. */
#define ph_color_buffer_alloc_fn_name "ph_color_buffer_alloc"

/******************************************************************************/
/**
Release the buffer previously allocated by #ph_color_buffer_alloc function.

This function will release memory allocated for buffer and sets @p pBuffer to
null.

@param[in,out] pBuffer      A location where a pointer to already allocated
                            buffer is stored.
*/
void PV_DECL ph_color_buffer_free(
        void** pBuffer);
/** For loading #ph_color_buffer_free at run-time. */
typedef void (PV_DECL *ph_color_buffer_free_fn)(
        void** pBuffer);
/** For loading #ph_color_buffer_free at run-time. */
#define ph_color_buffer_free_fn_name "ph_color_buffer_free"

#ifdef PV_C_PLUS_PLUS
};
#endif

#endif /* PVCAM_HELPER_COLOR_H */
