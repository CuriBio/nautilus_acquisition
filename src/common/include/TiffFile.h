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
            //ph_color_context* colorCtx{nullptr};
            Bitmap* bmp{nullptr};
            double fillValue{0.0};
        };

    private:
        std::string m_name{};
        TIFF* m_file{ nullptr };

        const uint32_t m_width;
        const uint32_t m_height;
        const uint16_t m_frameCount;

        /* const size_t m_rawData; */
        /* const size_t m_rawDataAligned; */

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
};


template<FrameConcept F>
TiffFile<F>::TiffFile(const Region& rgn, const ImageFormat format, uint16_t bitDepth, uint32_t frameCount) :
    m_width((rgn.sbin == 0) ? 0 : (rgn.s2 + 1 - rgn.s1) / rgn.sbin),
    m_height((rgn.pbin == 0) ? 0 : (rgn.p2 + 1 - rgn.p1) / rgn.pbin),
    m_frameCount(frameCount),
    m_format(format),
    m_bitDepth(bitDepth),
    m_useBigTiff(false),
    m_bmpFormat(BitmapFormat(format, bitDepth)) {
        //m_bmp = std::make_unique<Bitmap>(m_width, m_height, m_bmpFormat);
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

