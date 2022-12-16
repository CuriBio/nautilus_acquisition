/*
 * MIT License
 *
 * Copyright (c) 2022 Curi Bio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef BITMAP_FORMAT_H
#define BITMAP_FORMAT_H
#include <cstdint>
#include <cstddef>

enum class ImageFormat : int32_t {
    Mono16 = 0,     //16bit mono, 2 bytes per pixel.
    Bayer16 = 1,    // 16bit bayer masked image, 2 bytes per pixel.
    Mono8 = 2,      // 8bit mono, 1 byte per pixel.
    Bayer8 = 3,     // 8bit bayer masked image, 1 byte per pixel.
    Mono24 = 4,   // 24bit mono, 3 bytes per pixel.
    Bayer24 = 5,  // 24bit bayer masked image, 3 bytes per pixel.
    RGB24 = 6,      // 8bit RGB, 1 byte per sample, 3 bytes per pixel.
    RGB48 = 7,      // 16bit RGB, 2 bytes per sample, 6 bytes per pixel.
    RGB72 = 8,    // 24bit RGB, 3 bytes per sample, 9 bytes per pixel.
    Mono32 = 9,     // 32bit mono, 4 bytes per pixel.
    Bayer32 = 10,   // 32bit bayer masked image, 4 bytes per pixel.
    // TODO: Fix the value once defined in pvcam.h
    RGB96 = 11,     // 32bit RGB, 4 bytes per sample, 12 bytes per pixel.
};

enum class BitmapPixelType : int32_t {
    Mono = 0, // Each pixel contains only one sample
    RGB,      // Each pixel consists of 3 samples, Red, Green, Blue, in this order
};

enum class BitmapDataType : int32_t {
    UInt8 = 0,  // Each sample takes 1 byte, 8-bit unsigned value
    UInt16,     // Each sample takes 2 bytes, 16-bit unsigned value
    UInt32,     // Each sample takes 4 bytes, 32-bit unsigned value
};

/**
Bayer pattern for the Mono bitmap format. The pattern is named with 4 letters
where the first two are first line and another two are the next line.
For example RGGB means the pixels have following mask:
@code
R G R G R G ...
G B G B G B ...
R G R G R G ...
G B G B G B ...
...
@endcode
The values correspond to PL_COLOR_MODES from PVCAM.
*/
enum class BayerPattern : int32_t {
    None = 0, // No Bayer pattern, the image is plain monochrome bitmap
    RGGB = 2,
    GRBG = 3,
    GBRG = 4,
    BGGR = 5,
};

/**
A class that describes the format of a Bitmap.

Data type:         A type of the sample, usually UInt16 for 16bit camera images.
Samples-per-pixel: Technically, one pixel may consist of several samples, e.g.
                   RGB bitmaps have 3 samples per pixel, RGBA or CMYK bitmap
                   would have 4 samples per pixel. Gray-scale bitmaps have 1
                   sample per pixel.
Bits per sample:   Image bit depth, a 16-bit data type may have only 14, 10 or
                   even 8 bits valid.
                   Bits-per-sample is the actual bit depth. Data-type is the
                   sample carrier. An UInt32-type bitmap may have bit-depth of
                   12 only if it was, for example, up-converted from UInt16.

Example of a 16bit RGB frame:
          _______________________________________
SAMPLES: |  R  |  G  |  B  |  R  |  G  |  B  |...|
         |__0__|__1__|__2__|__3__|__4__|__5__|   |
PIXELS:  |        0        |        1        |   |
         |_________________|_________________|   |
BYTES:   |0 |1 |2 |3 |4 |5 |6 |7 |8 |9 |10|11|...|
*/
class BitmapFormat {
    private:
        ImageFormat m_imageFormat{ ImageFormat::Mono16 };
        BitmapPixelType m_pixelType{ BitmapPixelType::Mono };
        BitmapDataType m_dataType{ BitmapDataType::UInt16 };
        uint16_t m_bitDepth{ 16 };
        BayerPattern m_colorMask{ BayerPattern::None };

    public:
        BitmapFormat();
        BitmapFormat(ImageFormat imageFormat, uint16_t bitDepth);
        BitmapFormat(BitmapPixelType pixelType, BitmapDataType dataType, uint16_t bitDepth);

        ImageFormat GetImageFormat() const;
        void SetImageFormat(ImageFormat imageFormat);

        BitmapPixelType GetPixelType() const;
        void SetPixelType(BitmapPixelType pixelType);

        BitmapDataType GetDataType() const;
        void SetDataType(BitmapDataType dataType);

        uint16_t GetBitDepth() const;
        void SetBitDepth(uint16_t bitDepth);

        /**
          Returns the sensor, or bitmap mask used for this bitmap, for example RGGB or
          simply None. This value does not (and should not) have any effect on the
          bitmap data or its size. The value only tells the caller how to interpret
          the pixels. For example a monochromatic bitmap may have been acquired on a
          sensor with RGGB bayer mask. The bitmap configuration (and size) is no
          different from normal gray-scale bitmap but in order to properly display the
          bitmap it has to be demosaiced. The BayerPattern value is used to properly
          represent the bitmap on the screen.
          @return Bitmap color mask.
          */
        BayerPattern GetColorMask() const;
        void SetColorMask(BayerPattern colorMask);

        size_t GetBytesPerPixel() const;
        size_t GetBytesPerSample() const;
        uint8_t GetSamplesPerPixel() const;

    private:
        void SetupPixelAndDataType(ImageFormat imageFormat);
        void SetupImageFormat(BitmapPixelType pixelType, BitmapDataType dataType);

};

#endif //BITMAP_FORMAT_H
