#ifndef BITMAP_H
#define BITMAP_H
#include "BitmapFormat.h"

class Bitmap {
    private:
        uint8_t* m_data{ nullptr };
        size_t m_size{ 0 };

        // Format not const just to allow override of color mask
        BitmapFormat m_format{}; ///< Bitmap format (pixel type, data type, ...)

    public:
        const uint32_t width{0};
        const uint32_t height{0};
        const uint16_t alignment{ 1 };
        const size_t stride{0};

    public:
        Bitmap(void* data, uint32_t w, uint32_t h, const BitmapFormat& f, uint16_t lineAlign = 1);
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
