/******************************************************************************/
/* Copyright (C) Teledyne Photometrics. All rights reserved.                  */
/******************************************************************************/
#pragma once
#ifndef PVCAM_HELPER_TRACK_H
#define PVCAM_HELPER_TRACK_H

/* PVCAM */
#include <master.h>

/**
Major version number.

This part of version number changes whenever a binary incompatible modification
to library API is done, e.g. changed name or prototype of existing function.

Applications can check this at compile time to use appropriate syntax.
*/
#define PH_TRACK_VERSION_MAJOR 1
/**
Minor version number.

This part of version number changes whenever a binary compatible modification to
library API is done, e.g. added new function or data type.

Applications can check this at compile time to use new functions and/or types.
*/
#define PH_TRACK_VERSION_MINOR 2

/**
The context holds all internal data needed by library to work.

It is intentionally an opaque type because there is nothing to be modified by
application. Having all states bundled on one place allows for instance to work
with multiple cameras simultaneously without any synchronization on code level.
*/
typedef void* PH_TRACK_CONTEXT;

/** Invalid context value for safe initialization. */
#define PH_TRACK_CONTEXT_INVALID NULL

/** Maximum length of the error message including null terminator. */
#define PH_TRACK_MAX_ERROR_LEN 4096

/** Error codes returned by functions. */
typedef enum PH_TRACK_ERROR
{
    PH_TRACK_ERROR_NONE = 0,
    PH_TRACK_ERROR_GENERIC = 1,
    PH_TRACK_ERROR_ARGUMENT = 2,
    PH_TRACK_ERROR_ARGUMENT_NULL = 3,
    PH_TRACK_ERROR_ARGUMENT_OUT_OF_RANGE = 4,
    PH_TRACK_ERROR_NOT_SUPPORTED = 11,
    PH_TRACK_ERROR_OUT_OF_MEMORY = 13,
    PH_TRACK_ERROR_BUFFER_TOO_SMALL = 1000
}
PH_TRACK_ERROR;

/** Particle state recognized by library. */
typedef enum PH_TRACK_PARTICLE_STATE
{
    /** Particle has shown on this frame for first time. */
    PH_TRACK_PARTICLE_STATE_APPEARED = 0,

    /**
    Particle is present in this frame after a short drop
    (shorter than @p maxFramesToLink given to #ph_track_init).
    */
    PH_TRACK_PARTICLE_STATE_REAPPEARED,

    /** Particle found on this frame was also on previous frame. */
    PH_TRACK_PARTICLE_STATE_CONTINUATION,

}
PH_TRACK_PARTICLE_STATE;

/**
Structure for coordinates.

The coordinates are floating-point numbers needed by linking algorithm, but
usually @p uns16 values coming from PVCAM are assigned directly.
*/
typedef struct ph_track_particle_coord
{
    double x;
    double y;
}
ph_track_particle_coord;

/** Input values for linking collected from PVCAM frame metadata. */
typedef struct ph_track_particle_event
{
    /** The ROI number of particle - needed for back-mapping to image data. */
    uns16 roiNr;
    /** The center location of particle in sensor coordinates. */
    ph_track_particle_coord center;
    double m0;
    double m2;
}
ph_track_particle_event;

/** Output values from linking, one for each particle found. */
typedef struct ph_track_particle
{
    /** A copy of input data - needed for back-mapping to image data. */
    ph_track_particle_event event;

    /**
    Unique particle's ID.

    The helper library's primary responsibility is to maintain a consistent ID
    for a particle across multiple frames. The ID will be unique for each
    particle until the ID counter rolls over.
    */
    uns32 id;

    /**
    Particle's state.

    The value is one of those described #PH_TRACK_PARTICLE_STATE enumeration. */
    int32 state;

    /**
    Particle's lifetime.

    Provides the number of frames that this particle has been present for. When
    a particle's state is #PH_TRACK_PARTICLE_STATE_APPEARED, lifetime is always
    equal to 1. The @p lifetime increments by one with each frame in which it is
    detected. Applications may want to buffer results and wait until @p lifetime
    reaches several frames before accepting a particle in order to reduce false
    positives and support increased sensitivity. */
    uns32 lifetime;
}
ph_track_particle;

#ifdef PV_C_PLUS_PLUS
extern "C"
{
#endif

/******************************************************************************/
/**
Offers all three parts of the helper library version.

This function can be called at any time. It could fail only in case the NULL
pointer is given as parameter.

@param[out] pVerMajor   Major version number, returns at runtime the same as
                        macro #PH_TRACK_VERSION_MAJOR.
@param[out] pVerMinor   Minor version number, returns at runtime the same as
                        macro #PH_TRACK_VERSION_MINOR.
@param[out] pVerBuild   Build version number, provided for completeness.
                        It is always increasing number changed with every
                        modification in library.

@return Returns error code from #PH_TRACK_ERROR enumeration.
*/
int32 PV_DECL ph_track_get_lib_version(
        uns16* pVerMajor, uns16* pVerMinor, uns16* pVerBuild);
/** For loading #ph_track_get_lib_version at run-time. */
typedef int32 (PV_DECL *ph_track_get_lib_version_fn)(
        uns16* pVerMajor, uns16* pVerMinor, uns16* pVerBuild);
/** For loading #ph_track_get_lib_version at run-time. */
#define ph_track_get_lib_version_fn_name "ph_track_get_lib_version"

/******************************************************************************/
/**
Provides human-readable message describing last error.

After any function returns an error condition, #ph_track_get_last_error_message
can be called on the same thread to retrieve a more detailed, human-readable
error description string. The caller provides a buffer @p pBuffer to receive the
string of max. length @p bufferSize bytes.

If the error message exceeds @p bufferSize bytes, then error
#PH_TRACK_ERROR_BUFFER_TOO_SMALL is returned and @p bufferSize value is replaced
with the minimum necessary buffer size. A second call can then be made with a
buffer of the necessary size to retrieve the message.

If the buffer is too small, then the message is copied to the length available
even if the #PH_TRACK_ERROR_BUFFER_TOO_SMALL error code is returned.

A @p bufferSize of zero is allowed in order to query the required buffer size
for a subsequent call.

Maximum message size is limited by #PH_TRACK_MAX_ERROR_LEN value.

@param[out]    pBuffer      Output buffer filled with last error message.
@param[in,out] bufferSize   Size of given buffer @a pBuffer in bytes.

@return Returns error code from #PH_TRACK_ERROR enumeration.
*/
int32 PV_DECL ph_track_get_last_error_message(
        char* pBuffer, uns32* bufferSize);
/** For loading #ph_track_get_last_error_message at run-time. */
typedef int32 (PV_DECL *ph_track_get_last_error_message_fn)(
        char* pBuffer, uns32* bufferSize);
/** For loading #ph_track_get_last_error_message at run-time. */
#define ph_track_get_last_error_message_fn_name \
        "ph_track_get_last_error_message"

/******************************************************************************/
/**
Allocate and initialize the context used in the helper library.

After #ph_track_init is called, the #ph_track_link_particles function can be
used by providing the context. When done, the provided context should be passed
to #ph_track_uninit to free the memory.

A new context should be initialized for each sequence acquired to clear the
tracking state.

@param[out] pContext            A location where newly allocated context will be
                                stored.
@param[in]  maxFramesToLink     The maximum number of frames to establish
                                linkage between. For example, using value 2 will
                                link particles only between consecutive frames.
                                Using @a maxFramesToLink of 3 will allow frames
                                to disappear for one frame and become
                                re-established one frame later. The minimum
                                value is 2. Larger values of @a maxFramesToLink
                                will utilize more computational and memory
                                resources and scales as O(n^2).
                                Values larger than 5 are not recommended.
@param[in]  maxDistancePerFrame The maximum number of pixels to allow a linkage
                                between per frame. For example, if an event is
                                identified at location (100,100) in one frame,
                                then a @a maxDistancePerFrame would allow an
                                event at (150,100) on the subsequent frame or an
                                event at (200,100) on a 2nd frame to be linked
                                to that first event, if the value is 50.
                                The value should be higher for faster moving
                                particles, but larger values can also lead to
                                incorrect linkages if the event density is high
                                enough for mismatches to occur. A value of 50
                                pixels is suggested for most applications.
                                This parameter can range from 0 to the full
                                diagonal dimension of the sensor, and is
                                specified in units of pixels.
@param[in]  forceCpuOnly        The library automatically utilizes a
                                CUDA-compatible GPU if one is found in the
                                system, unless the @a forceCpuOnly flag is set.
                                If multiple compatible GPUs are found, the one
                                with the highest compute capability is used.
                                If no compatible GPU is found, the library will
                                only utilize the system's CPU.
@param[in]  maxCentroidsCount   The number of centroids used to setup PVCAM
                                acquisition.
@param[out] pMaxOutputParticles The number of elements that must be provided in
                                the @p pParticles array argument passed to
                                the #ph_track_link_particles call.

@return Returns error code from #PH_TRACK_ERROR enumeration.
*/
int32 PV_DECL ph_track_init(
        PH_TRACK_CONTEXT* pContext,
        uns16 maxFramesToLink, uns16 maxDistancePerFrame, rs_bool forceCpuOnly,
        uns32 maxCentroidsCount, uns32* pMaxOutputParticles);
/** For loading #ph_track_init at run-time. */
typedef int32 (PV_DECL *ph_track_init_fn)(
        PH_TRACK_CONTEXT* pContext,
        uns16 maxFramesToLink, uns16 maxDistancePerFrame, rs_bool forceCpuOnly,
        uns32 maxCentroidsCount, uns32* pMaxOutputParticles);
/** For loading #ph_track_init at run-time. */
#define ph_track_init_fn_name "ph_track_init"

/******************************************************************************/
/**
Link particles across the frames.

Link particles from latest frame with those found in previous frames (previous
calls). Link particles will provide an array @a pParticles of
@p ph_track_particle elements, and will establish consistent particle ID numbers
across camera frames as well as state and lifetime information on each particle
being tracked.

@param[in]     context          A context previously created by #ph_track_init.
@param[in]     pEvents          A pointer to an array containing @a eventsCount
                                elements of the #ph_track_particle_event
                                structure. This array describes all detection
                                events, as provided by camera metadata.
@param[in]     eventsCount      The number of elements in the @a pEvents array
                                for this frame.
@param[out]    pParticles       An array provided by the caller that will
                                receive the results of linking operation. The
                                array must contain at least
                                @p pMaxOutputParticles elements returned by
                                #ph_track_init.
@param[in,out] pParticlesCount  The count must be set by the caller to indicate
                                the number of elements available in the
                                @a pParticles array. It must be at least as
                                large as the @p pMaxOutputParticles value
                                received from #ph_track_init, otherwise an error
                                #PH_TRACK_ERROR_BUFFER_TOO_SMALL will be
                                returned. Upon return from function, this value
                                will be updated to contain the number of
                                elements of the array that are valid and
                                utilized on this frame.

@return Returns error code from #PH_TRACK_ERROR enumeration.
*/
int32 PV_DECL ph_track_link_particles(
        PH_TRACK_CONTEXT context,
        const ph_track_particle_event* pEvents, uns32 eventsCount,
        ph_track_particle* pParticles, uns32* pParticlesCount);
/** For loading #ph_track_link_particles at run-time. */
typedef int32 (PV_DECL *ph_track_link_particles_fn)(
        PH_TRACK_CONTEXT context,
        const ph_track_particle_event* pEvents, uns32 eventsCount,
        ph_track_particle* pParticles, uns32* pParticlesCount);
/** For loading #ph_track_link_particles at run-time. */
#define ph_track_link_particles_fn_name "ph_track_link_particles"

/******************************************************************************/
/**
Release the context previously created by #ph_track_init function.

This function will release memory allocated for context and sets @p pContext to
null.

@param[in,out] pContext     Location where already allocated context is stored.

@return Returns error code from #PH_TRACK_ERROR enumeration.
*/
int32 PV_DECL ph_track_uninit(PH_TRACK_CONTEXT* pContext);
/** For loading #ph_track_uninit at run-time. */
typedef int32 (PV_DECL *ph_track_uninit_fn)(PH_TRACK_CONTEXT* pContext);
/** For loading #ph_track_uninit at run-time. */
#define ph_track_uninit_fn_name "ph_track_uninit"

/******************************************************************************/
/**
Provides error message for all known error codes.

This function returns generic error message based on given error code. It never
fails and can be called at any time, event before #ph_track_init.

Instead, it is recommented to call #ph_track_get_last_error_message right after
an error occured to get more detailed error message.

The message length returned by this function is not limited to
#PH_TRACK_MAX_ERROR_LEN, but it is usually very short.

@param[in]  errorCode    An error code - value from #PH_TRACK_ERROR enum.

@return Returns generic message for given error code.
*/
const char* PV_DECL ph_track_get_error_message(int32 errorCode);
/** For loading ph_track_get_error_message at run-time. */
typedef const char* (PV_DECL *ph_track_get_error_message_fn)(int32 errorCode);
/** For loading ph_track_get_error_message at run-time. */
#define ph_track_get_error_message_fn_name "ph_track_get_error_message"

#ifdef PV_C_PLUS_PLUS
};
#endif

#endif /* PVCAM_HELPER_TRACK_H */
