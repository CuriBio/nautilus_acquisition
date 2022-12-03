#include <stdexcept>
#include "Bitmap.h"

Bitmap::Bitmap(void* data, uint32_t w, uint32_t h, const BitmapFormat& fmt, uint16_t align)
    : width(w),
    height(h),
    m_data(static_cast<uint8_t*>(data)),
    alignment(align),
    stride(CalculateStrideBytes(w, fmt, align)),
    m_deleteData(false)
{
    m_size = stride * height;
}

Bitmap::Bitmap(uint32_t w, uint32_t h, const BitmapFormat& fmt, uint16_t align)
    : width(w),
    height(h),
    alignment(align),
    stride(CalculateStrideBytes(w, fmt, align)),
    m_deleteData(true)
{
    m_size = stride * height;
    m_data = new uint8_t[m_size];
}

Bitmap::~Bitmap() {
    if (m_deleteData) {
        delete m_data;
    }
}

void* Bitmap::GetData() const {
    return m_data;
}

size_t Bitmap::GetDataBytes() const {
    return m_size;
}

const BitmapFormat& Bitmap::GetFormat() const {
    return m_format;
}

void Bitmap::ChangeColorMask(BayerPattern colorMask) {
    m_format.SetColorMask(colorMask);
}

void* Bitmap::GetScanLine(uint16_t y) const {
    return m_data + y * stride;
}

double Bitmap::GetSample(uint16_t x, uint16_t y, uint8_t sIdx) const {
    const uint8_t spp = m_format.GetSamplesPerPixel();
    const void* const scanLine = GetScanLine(y);
    const size_t pos = (size_t)x * spp + sIdx;

    switch (m_format.GetDataType()) {
        case BitmapDataType::UInt8:
            return (static_cast<const uint8_t*>(scanLine))[pos];
        case BitmapDataType::UInt16:
            return (static_cast<const uint16_t*>(scanLine))[pos];
        case BitmapDataType::UInt32:
            return (static_cast<const uint32_t*>(scanLine))[pos];
        default:
            throw std::invalid_argument("Unsupported bitmap data type");
    }
}

size_t Bitmap::CalculateStrideBytes(uint32_t w, const BitmapFormat& fmt, uint16_t align) {
    const size_t bytesPerPixel = fmt.GetBytesPerPixel();
    const size_t bytesPerLine = w * bytesPerPixel;
    return ((bytesPerLine + align - 1) / align) * align;
}

