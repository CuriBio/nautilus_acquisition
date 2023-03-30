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
 * Definition of the TiffFile class.
 *********************************************************************/
#ifndef TIFF_FILE_H
#define TIFF_FILE_H
#include <string>
#include <cstdint>
#include <cstddef>
#include <iterator>

#include <tiffio.h>

#include <interfaces/FileInterface.h>
#include <interfaces/FrameInterface.h>
#include <interfaces/AcquisitionInterface.h>
#include <Bitmap.h>


/*
* Tiff file writer.
*
* @tparam F FrameConcept implementation.
*/
class TiffFile {
    public:
        struct ProcHelper {
            Bitmap* bmp{nullptr};
            double fillValue{0.0};
        };

    private:
        std::string m_name{};
        TIFF* m_file{ nullptr };

        uint32_t m_width{0};
        uint32_t m_height{0};
        uint16_t m_frameCount{0};

        bool m_useBigTiff{true};
        ImageFormat m_format;
        BitmapFormat m_bmpFormat;
        uint16_t m_bitDepth;

        //subifd
        uint32_t*   m_subifd_offsets{nullptr};

        std::unique_ptr<Bitmap> m_bmp{nullptr};
        uint32_t m_frameIndex{0};

    public:
        /*
         * TiffFile constructor.
         *
         * @params rgn The image capture region.
         * @param format The image format.
         * @param bitDepth Image pixel bitdepth.
         * @param frameCount The number of frames in this image, 1 unless using tiffstack.
         */
        TiffFile(const Region& rgn, const ImageFormat format, uint16_t bitDepth, uint32_t frameCount);

        /*
         * TiffFile constructor. Load from file.
         *
         * @param file The path to the tiff file.
         */
        TiffFile(std::string file);

        /*
         * TiffFile constructor
         */
        TiffFile(std::filesystem::path outp, uint32_t width, uint32_t height, uint16_t bitDepth, uint16_t frameCount, bool bigTiff);

        /*
         * TiffFile destructor.
         */
        ~TiffFile();

        /*
         * Open tiff file.
         *
         * @param name File name.
         *
         * @return true if successful, false otherwise.
         */
        bool Open(std::string name);

        /*
         * Close tiff file, will flush to disk.
         */
        void Close();

        /*
         * Check if file is open.
         *
         * @return true if open, false otherwise.
         */
        bool IsOpen() const;

        /*
         * File name.
         *
         * @return Name of file.
         */
        const std::string& Name() const;

        /*
         * Write frame data to file.
         *
         * @param frame Pointer to frame.
         *
         * @return true if successful, false otherwise.
         */
        template<FrameConcept F>
        bool WriteFrame(F* frame);

        /*
         * Write raw buffer
         */
        bool Write16(uint16_t* data);

        bool Read16(uint16_t* data, uint32_t row) {
             TIFFReadScanline(m_file, data, row, 0);
             return true;
        }

        /*
         * Helper function to load tiff file data.
         *
         * @param path Path to tiff file.
         * @param width returns width of image.
         * @param height returns height of image.
         *
         * @return uint16_t Pointer to image data.
         */
        static uint16_t* LoadTIFF(const char* path, uint32_t& width, uint32_t& height);
};

template<FrameConcept F>
bool TiffFile::WriteFrame(F* frame) {
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

