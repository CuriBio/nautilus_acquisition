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

/*********************************************************************
 * @file  TiffFile.h
 * 
 * @brief Definition of the TiffFile class.
 *********************************************************************/
#ifndef TIFF_FILE_H
#define TIFF_FILE_H
#include <string>
#include <cstdint>
#include <cstddef>
#include <tiffio.h>

#include <interfaces/FileInterface.h>
#include <interfaces/FrameInterface.h>
#include <interfaces/AcquisitionInterface.h>
#include <Bitmap.h>

template<FrameConcept F>
class TiffFile {
    public:
        struct ProcHelper {
            Bitmap* bmp{nullptr};
            double fillValue{0.0};
        };

    private:
        std::string m_name{};
        TIFF* m_file{ nullptr };

        const uint32_t m_width;
        const uint32_t m_height;
        const uint16_t m_frameCount;

        const bool m_useBigTiff;
        const ImageFormat m_format;
        const BitmapFormat m_bmpFormat;
        const uint16_t m_bitDepth;

        std::unique_ptr<Bitmap> m_bmp{nullptr};
        uint32_t m_frameIndex{0};

    public:
        TiffFile(const Region& rgn, const ImageFormat format, uint16_t bitDepth, uint32_t frameCount);
        ~TiffFile();

        bool Open(std::string name);
        void Close();
        bool IsOpen() const;
        const std::string& Name() const;
        bool WriteFrame(F* frame);

        static uint16_t* LoadTIFF(const char* path, uint32_t& width, uint32_t& height);
};

template<FrameConcept F>
uint16_t* TiffFile<F>::LoadTIFF(const char* path, uint32_t& width, uint32_t& height) {
    // Temporary variables
    uint16_t* data;

    //Loads tiff file
    TIFF* tiff = TIFFOpen(path, "r");
    if(!tiff) {
    spdlog::error("Failed to read TIFF: {}", path);
        return nullptr;
    }

    // Read dimensions of image
    if (TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width) != 1) {
        spdlog::error("Failed to read width of TIFF: {}", path);
    }
    if (TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height) != 1) {
        spdlog::error("Failed to read height of TIFF: {}", path);
    }

    //allocate data buffer
    data = new uint16_t[width*height];
    memset((void*)data, 0, sizeof(uint16_t)*width*height);

    for (uint32_t y=0; y<height; ++y) {
         TIFFReadScanline(tiff, data+(y*width), y, 0);
         uint16_t* end = data+(y*width)+width;
    }

    //close tiff file
    TIFFClose(tiff);

  return data;
}



template<FrameConcept F>
TiffFile<F>::TiffFile(const Region& rgn, const ImageFormat format, uint16_t bitDepth, uint32_t frameCount) :
    m_width((rgn.sbin == 0) ? 0 : (rgn.s2 + 1 - rgn.s1) / rgn.sbin),
    m_height((rgn.pbin == 0) ? 0 : (rgn.p2 + 1 - rgn.p1) / rgn.pbin),
    m_frameCount(frameCount),
    m_format(format),
    m_bitDepth(bitDepth),
    m_useBigTiff(false),
    m_bmpFormat(BitmapFormat(format, bitDepth)) {
}

template<FrameConcept F>
TiffFile<F>::~TiffFile() {
}

template<FrameConcept F>
bool TiffFile<F>::Open(std::string name) {
    m_name = name;
    const char* mode = (m_useBigTiff) ? "w8" : "w";
    m_file = TIFFOpen(m_name.c_str(), mode);

    //TODO error handling
    return true;
}

template<FrameConcept F>
void TiffFile<F>::Close() {
    if (m_file) {
        TIFFFlush(m_file);
        TIFFClose(m_file);
        m_file = nullptr;
    }
}

template<FrameConcept F>
bool TiffFile<F>::IsOpen() const {
    return !!m_file;
}

template<FrameConcept F>
const std::string& TiffFile<F>::Name() const {
    return m_name;
}

template<FrameConcept F>
bool TiffFile<F>::WriteFrame(F* frame) {
    if (!IsOpen()) {
        return false;
    }

    //TODO check rawData length
    if (m_width == 0 || m_height == 0) { //|| m_rawData == 0) {
        return false;
    }

    Bitmap bmp(frame->GetData(), m_width, m_height, m_bmpFormat);

    TIFFSetField(m_file, TIFFTAG_IMAGEWIDTH, m_width);
    TIFFSetField(m_file, TIFFTAG_IMAGELENGTH, m_height);
    TIFFSetField(m_file, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
    TIFFSetField(m_file, TIFFTAG_BITSPERSAMPLE, 8 * m_bmpFormat.GetBytesPerSample());
    TIFFSetField(m_file, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(m_file, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(m_file, TIFFTAG_SAMPLESPERPIXEL, m_bmpFormat.GetSamplesPerPixel());
    TIFFSetField(m_file, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

    if (m_bmpFormat.GetBitDepth() <= 16) {
        TIFFSetField(m_file, TIFFTAG_MAXSAMPLEVALUE, (1u << m_bmpFormat.GetBitDepth()) - 1);
    }

    if (m_frameCount > 1) {
        // We are writing single page of the multi-page file
        TIFFSetField(m_file, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
        // Set the page number
        TIFFSetField(m_file, TIFFTAG_PAGENUMBER, m_frameIndex, m_frameCount);
    }

    /* // TODO Put the PVCAM metadata into the image description */
    TIFFSetField(m_file, TIFFTAG_IMAGEDESCRIPTION, m_name.c_str());

    auto tiffData = bmp.GetData();
    const auto tiffDataBytes = bmp.GetDataBytes();

    // This is fastest streaming option, but it requires the TIFFTAG_ROWSPERSTRIP
    // tag is not set (or maybe requires well calculated value)
    if (tiffDataBytes != (size_t)TIFFWriteRawStrip(m_file, 0, tiffData, tiffDataBytes)) {
        return false;
    }

    if (m_frameCount > 1) {
        TIFFWriteDirectory(m_file);
    }

    m_frameIndex++;
    return true;
}
#endif //TIFF_FILE_H

