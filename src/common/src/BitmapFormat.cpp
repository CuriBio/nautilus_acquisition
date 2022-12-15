#include <stdexcept>
#include <BitmapFormat.h>

BitmapFormat::BitmapFormat() {
}

BitmapFormat::BitmapFormat(ImageFormat imageFormat, uint16_t bitDepth)
    : m_imageFormat(imageFormat), m_bitDepth(bitDepth)
{
    SetupPixelAndDataType(imageFormat);
}

BitmapFormat::BitmapFormat(BitmapPixelType pixelType, BitmapDataType dataType, uint16_t bitDepth)
 : m_pixelType(pixelType), m_dataType(dataType), m_bitDepth(bitDepth)
{
    SetupImageFormat(pixelType, dataType);
}

ImageFormat BitmapFormat::GetImageFormat() const {
    return m_imageFormat;
}

void BitmapFormat::SetImageFormat(ImageFormat imageFormat) {
    SetupPixelAndDataType(imageFormat);
}

BitmapPixelType BitmapFormat::GetPixelType() const {
    return m_pixelType;
}

void BitmapFormat::SetPixelType(BitmapPixelType pixelType) {
    SetupImageFormat(pixelType, m_dataType);
    m_pixelType = pixelType;
}

BitmapDataType BitmapFormat::GetDataType() const {
    return m_dataType;
}

void BitmapFormat::SetDataType(BitmapDataType dataType) {
    SetupImageFormat(m_pixelType, dataType);
    m_dataType = dataType;
}

uint16_t BitmapFormat::GetBitDepth() const
{
    return m_bitDepth;
}

BayerPattern BitmapFormat::GetColorMask() const {
    return m_colorMask;
}

void BitmapFormat::SetColorMask(BayerPattern colorMask) {
    m_colorMask = colorMask;
}

size_t BitmapFormat::GetBytesPerPixel() const {
    const uint8_t samplesPerPixel = GetSamplesPerPixel();
    const size_t bytesPerSample = GetBytesPerSample();
    return samplesPerPixel * bytesPerSample;
}

size_t BitmapFormat::GetBytesPerSample() const {
    switch (m_dataType)
    {
        case BitmapDataType::UInt8:
            return sizeof(uint8_t);
        case BitmapDataType::UInt16:
            return sizeof(uint16_t);
        case BitmapDataType::UInt32:
            return sizeof(uint32_t);
        default:
            return 0;
    }
}

uint8_t BitmapFormat::GetSamplesPerPixel() const {
    switch (m_pixelType)
    {
        case BitmapPixelType::Mono:
            return 1;
        case BitmapPixelType::RGB:
            return 3;
        default:
            return 0;
    }
}

void BitmapFormat::SetupPixelAndDataType(ImageFormat imageFormat) {
    switch (imageFormat)
    {
        case ImageFormat::Mono8:
        case ImageFormat::Bayer8:
            m_dataType = BitmapDataType::UInt8;
            m_pixelType = BitmapPixelType::Mono;
            return;
        case ImageFormat::Mono16:
        case ImageFormat::Bayer16:
            m_dataType = BitmapDataType::UInt16;
            m_pixelType = BitmapPixelType::Mono;
            return;
        case ImageFormat::Mono32:
        case ImageFormat::Bayer32:
            m_dataType = BitmapDataType::UInt32;
            m_pixelType = BitmapPixelType::Mono;
            return;
        case ImageFormat::RGB24:
            m_dataType = BitmapDataType::UInt8;
            m_pixelType = BitmapPixelType::RGB;
            return;
        case ImageFormat::RGB48:
            m_dataType = BitmapDataType::UInt16;
            m_pixelType = BitmapPixelType::RGB;
            return;
        case ImageFormat::RGB96:
            m_dataType = BitmapDataType::UInt32;
            m_pixelType = BitmapPixelType::RGB;
            return;
        default:
            throw std::invalid_argument("Unsupported image format");
    }
}

void BitmapFormat::SetupImageFormat(BitmapPixelType pixelType, BitmapDataType dataType) {
    switch (dataType)
    {
        case BitmapDataType::UInt8:
            switch (pixelType)
            {
                case BitmapPixelType::Mono:
                    m_imageFormat = ImageFormat::Mono8;
                    return;
                case BitmapPixelType::RGB:
                    m_imageFormat = ImageFormat::RGB24;
                    return;
            }
            break; // Throws below

        case BitmapDataType::UInt16:
            switch (pixelType)
            {
                case BitmapPixelType::Mono:
                    m_imageFormat = ImageFormat::Mono16;
                    return;
                case BitmapPixelType::RGB:
                    m_imageFormat = ImageFormat::RGB48;
                    return;
            }
            break; // Throws below

        case BitmapDataType::UInt32:
            switch (pixelType)
            {
                case BitmapPixelType::Mono:
                    m_imageFormat = ImageFormat::Mono32;
                    return;
                case BitmapPixelType::RGB:
                    m_imageFormat = ImageFormat::RGB96;
                    return;
            }
            break; // Throws below

        default:
            throw std::invalid_argument("Unsupported bitmap data type");
    }
    throw std::invalid_argument("Unsupported pixel data type");
}

