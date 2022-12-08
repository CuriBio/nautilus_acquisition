#ifndef TASK_FRAMESTATS_H
#define TASK_FRAMESTATS_H

#include <mutex>
#include <vector>
#include <ranges>

#include <Bitmap.h>
#include <BitmapFormat.h>

class TaskFrameStats {
    private:
        std::mutex m_lock;

        uint32_t m_width, m_height;
        uint16_t m_min, m_max;
        uint16_t* m_data;

        /* std::vector<std::tuple<uint32_t, uint32_t>> m_stats; */
        /* Bitmap* m_bmp; */

    public:
        TaskFrameStats() {
            m_width = 0; m_height = 0;
            m_min = (1 << 16) - 1; m_max = 0;
            m_data = nullptr;
        };

        ~TaskFrameStats() = default;

        void Setup(uint16_t* data, uint32_t width, uint32_t height) {
            std::unique_lock<std::mutex> lock(m_lock);
            m_width = width;
            m_height = height;
            m_data = data;
        };

        void Results(uint16_t& min, uint16_t& max) {
            std::unique_lock<std::mutex> lock(m_lock);
            min = m_min; max = m_max;
        }

        void Run(uint8_t threadCount, uint8_t taskNum) {
            uint16_t min = (1 << 16) - 1;
            uint16_t max = 0;
            size_t rem = 0;

            if (taskNum == threadCount - 1) {
                rem = m_height % threadCount;
            }

            const size_t rows = rem + (m_height / threadCount);
            const size_t rowOffset = taskNum * rows;
            const size_t offset = rowOffset * m_width;


            for (uint16_t* start = m_data+offset; start < m_data+offset+(rows*m_width); start++) {
                if (*start > max) { max = *start; }
                if (*start < min) { min = *start; }
            }

            std::unique_lock<std::mutex> lock(m_lock);
            if (min < m_min) { m_min = min; }
            if (max > m_max) { m_max = max; }
        }

        /* void Run(uint8_t threadCount, uint8_t taskNum) { */
        /*     const uint32_t lineStep = static_cast<uint32_t>(threadCount); */
        /*     const uint32_t lineOff = static_cast<uint32_t>(taskNum); */

        /*     switch (m_bmp->GetFormat().GetImageFormat()) { */
        /*         case ImageFormat::Mono16: */
        /*             { */
        /*                 uint16_t min = (1 << 16) - 1; */
        /*                 uint16_t max = 0; */ 

        /*                 for (uint32_t y = lineOff; y < m_height; y += lineStep) { */
        /*                     const uint16_t* const srcLine = static_cast<const uint16_t*>(m_bmp->GetScanLine((uint16_t)y)); */
        /*                     minmax<uint16_t>(srcLine, min, max); */
        /*                 } */
        /*                 { */
        /*                     std::unique_lock<std::mutex> lock(m_lock); */
        /*                     //m_stats.push_back(std::make_tuple<uint32_t, uint32_t>(static_cast<uint32_t>(min), static_cast<uint32_t>(max))); */
        /*                 } */
        /*             } */
        /*             break; */
        /*         default: */
        /*             break; */
        /*     }; */
        /* } */

    private:
        template<typename T>
        void minmax(const T* const scanLine, T& min, T& max) {
            for (size_t n = 0; n < m_width; n++) {
                if (scanLine[n] > max) { max = scanLine[n]; }
                if (scanLine[n] < min) { min = scanLine[n]; }
            }
        }
};
#endif //TASK_FRAMESTATS_H
