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
 * @file  BitmapFormat.cpp
 * 
 * @brief Implementation of the BitmapFormat class.
 *********************************************************************/
#include <stdexcept>
#include <BitmapFormat.h>


/*
* @breif
*
*
*
* @param
*/
BitmapFormat::BitmapFormat() {
}


/*
* @breif
*
*
*
* @param
*/
BitmapFormat::BitmapFormat(ImageFormat imageFormat, uint16_t bitDepth)
    : m_imageFormat(imageFormat), m_bitDepth(bitDepth)
{
    SetupPixelAndDataType(imageFormat);
}


/*
* @breif
*
*
*
* @param
*/
BitmapFormat::BitmapFormat(BitmapPixelType pixelType, BitmapDataType dataType, uint16_t bitDepth)
 : m_pixelType(pixelType), m_dataType(dataType), m_bitDepth(bitDepth)
{
    SetupImageFormat(pixelType, dataType);
}


/*
* @breif
*
*
*
* @param
*/
ImageFormat BitmapFormat::GetImageFormat() const {
    return m_imageFormat;
}


/*
* @breif
*
*
*
* @param
*/
void BitmapFormat::SetImageFormat(ImageFormat imageFormat) {
    SetupPixelAndDataType(imageFormat);
}


/*
* @breif
*
*
*
* @param
*/
BitmapPixelType BitmapFormat::GetPixelType() const {
    return m_pixelType;
}


/*
* @breif
*
*
*
* @param
*/
void BitmapFormat::SetPixelType(BitmapPixelType pixelType) {
    SetupImageFormat(pixelType, m_dataType);
    m_pixelType = pixelType;
}


/*
* @breif
*
*
*
* @param
*/
BitmapDataType BitmapFormat::GetDataType() const {
    return m_dataType;
}


/*
* @breif
*
*
*
* @param
*/
void BitmapFormat::SetDataType(BitmapDataType dataType) {
    SetupImageFormat(m_pixelType, dataType);
    m_dataType = dataType;
}


/*
* @breif
*
*
*
* @param
*/
uint16_t BitmapFormat::GetBitDepth() const {
    return m_bitDepth;
}


/*
* @breif
*
*
*
* @param
*/
BayerPattern BitmapFormat::GetColorMask() const {
    return m_colorMask;
}


/*
* @breif
*
*
*
* @param
*/
void BitmapFormat::SetColorMask(BayerPattern colorMask) {
    m_colorMask = colorMask;
}


/*
* @breif
*
*
*
* @param
*/
size_t BitmapFormat::GetBytesPerPixel() const {
    const uint8_t samplesPerPixel = GetSamplesPerPixel();
    const size_t bytesPerSample = GetBytesPerSample();
    return samplesPerPixel * bytesPerSample;
}


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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


/*
* @breif
*
*
*
* @param
*/
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

