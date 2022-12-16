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
 * @file  Bitmap.h
 * 
 * @brief Definition of the Bitmap class.
 *********************************************************************/
#ifndef BITMAP_H
#define BITMAP_H
#include "BitmapFormat.h"

class Bitmap {
    private:
        uint8_t* m_data{ nullptr };
        size_t m_size{ 0 };
        bool m_deleteData{false};

        // Format not const just to allow override of color mask
        BitmapFormat m_format{}; ///< Bitmap format (pixel type, data type, ...)

    public:
        const uint32_t width{0};
        const uint32_t height{0};
        const uint16_t alignment{ 1 };
        const size_t stride{0};

    public:
        Bitmap(void* data, uint32_t w, uint32_t h, const BitmapFormat& f, uint16_t lineAlign = 1);
        Bitmap(void* data, uint32_t w, uint32_t h, const ImageFormat& fmt, int16_t bitDepth, uint16_t align=1);
        Bitmap(uint32_t w, uint32_t h, const BitmapFormat& f, uint16_t lineAlign = 1);
        ~Bitmap();

        void* GetData() const;
        size_t GetDataBytes() const;

        const BitmapFormat& GetFormat() const;
        void ChangeColorMask(BayerPattern colorMask);

        void* GetScanLine(uint16_t y) const;
        double GetSample(uint16_t x, uint16_t y, uint8_t sIdx = 0) const;

    private:
        size_t CalculateStrideBytes(uint32_t w, const BitmapFormat& fmt, uint16_t align);
};
#endif //BITMAP_H
