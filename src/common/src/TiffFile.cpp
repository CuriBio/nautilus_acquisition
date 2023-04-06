#include <spdlog/spdlog.h>

#include <TiffFile.h>


TiffFile::TiffFile(const Region& rgn, const ImageFormat format, uint16_t bitDepth, uint32_t frameCount) :
    m_width((rgn.sbin == 0) ? 0 : (rgn.s2 + 1 - rgn.s1) / rgn.sbin),
    m_height((rgn.pbin == 0) ? 0 : (rgn.p2 + 1 - rgn.p1) / rgn.pbin),
    m_frameCount(frameCount),
    m_format(format),
    m_bitDepth(bitDepth),
    m_useBigTiff((frameCount > 1) ? true : false), //Use BigTiff when storage type is TiffStack
    m_bmpFormat(BitmapFormat(format, bitDepth)) {
}

TiffFile::TiffFile(std::string file) {
    uint32_t width{0}, height{0};
    uint16_t frameCount{0};
    m_name = file;

    m_file = TIFFOpen(m_name.c_str(), "r");
    if (!m_file) {
        spdlog::error("Failed to open {}", file);
    }

    //TIFFPrintDirectory(m_file, stdout, 1);
    if (TIFFGetField(m_file, TIFFTAG_IMAGEWIDTH, &m_width) != 1) {
        spdlog::error("Failed to read width of TIFF: {}", file);
    }

    if (TIFFGetField(m_file, TIFFTAG_IMAGELENGTH, &m_height) != 1) {
        spdlog::error("Failed to read height of TIFF: {}", file);
    }

    uint16_t bitsPerSample{0};
    if (TIFFGetField(m_file, TIFFTAG_BITSPERSAMPLE, &bitsPerSample) != 1) {
        spdlog::error("Failed to bits per sample for TIFF: {}", file);
    }

    uint16_t samplesPerPixel{0};
    if (TIFFGetField(m_file, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel) != 1) {
        spdlog::error("Failed to bits per sample for TIFF: {}", file);
    }
}

TiffFile::TiffFile(std::filesystem::path outp, uint32_t width, uint32_t height, uint16_t bitDepth, uint16_t frameCount, bool bigTiff) :
    m_width(width),
    m_height(height),
    m_bitDepth(bitDepth),
    m_frameCount(frameCount),
    m_useBigTiff(bigTiff),
    m_frameIndex(0),
    m_name(outp.string())
{
    const char* mode = (m_useBigTiff) ? "w8" : "w";
    m_file = TIFFOpen(m_name.c_str(), mode);
}


TiffFile::~TiffFile() {
}


uint16_t* TiffFile::LoadTIFF(const char* path, uint32_t& width, uint32_t& height) {
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


bool TiffFile::Open(std::string name) {
    m_name = name;
    const char* mode = (m_useBigTiff) ? "w8" : "w";
    m_file = TIFFOpen(m_name.c_str(), mode);

    //TODO error handling
    return true;
}


void TiffFile::Close() {
    if (m_file) {
        TIFFFlush(m_file);
        TIFFClose(m_file);
        m_file = nullptr;
    }
}

bool TiffFile::Write(void* data, size_t frameIndex) {
    size_t bytes = m_width * m_height * (m_bitDepth / 8);
    TIFFSetField(m_file, TIFFTAG_IMAGEDESCRIPTION, m_name.c_str());
    TIFFSetField(m_file, TIFFTAG_IMAGEWIDTH, m_width);
    TIFFSetField(m_file, TIFFTAG_IMAGELENGTH, m_height);
    TIFFSetField(m_file, TIFFTAG_BITSPERSAMPLE, m_bitDepth);
    TIFFSetField(m_file, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
    TIFFSetField(m_file, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(m_file, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(m_file, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(m_file, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(m_file, TIFFTAG_MAXSAMPLEVALUE, (1u << m_bitDepth) - 1);

    /* // TODO Put the PVCAM metadata into the image description */
    if (m_frameCount > 1) {
        // We are writing single page of the multi-page file
        TIFFSetField(m_file, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
        // Set the page number
        TIFFSetField(m_file, TIFFTAG_PAGENUMBER, frameIndex, m_frameCount);
    }

    // This is fastest streaming option, but it requires the TIFFTAG_ROWSPERSTRIP
    // tag is not set (or maybe requires well calculated value)
    if (bytes != (size_t)TIFFWriteRawStrip(m_file, 0, data, bytes)) {
        return false;
    }


    if (m_frameCount > 1) {
        TIFFWriteDirectory(m_file);
    }

    m_frameIndex++;
    return true;
}


bool TiffFile::IsOpen() const {
    return !!m_file;
}


const std::string& TiffFile::Name() const {
    return m_name;
}
