#ifndef TASK_FRAMESTATS_H
#define TASK_FRAMESTATS_H

#include <mutex>
#include <vector>
#include <ranges>

#include <Bitmap.h>
#include <BitmapFormat.h>

class TaskFrameStats {
    private:
        uint32_t m_width, m_height;
        std::mutex m_lock;
        std::vector<std::tuple<uint32_t, uint32_t>> m_stats;
        Bitmap* m_bmp;

    public:
        TaskFrameStats() {
            m_width = 0;
            m_height = 0;
            m_bmp = nullptr;
        };

        ~TaskFrameStats() = default;

        void Setup(Bitmap* bmp) {
            std::unique_lock<std::mutex> lock(m_lock);
            m_stats.clear();
            m_width = bmp->width;
            m_height = bmp->height;
            m_bmp = bmp;
        };

        void Results(uint32_t& min, uint32_t& max) {
            std::unique_lock<std::mutex> lock(m_lock);

            min = static_cast<uint32_t>((1 << 31)); max = 0;
            /* for ( auto &n : m_stats) { */
            /*     if (std::get<0>(n) < min) { min = std::get<0>(n); } */
            /*     if (std::get<1>(n) > max) { max = std::get<1>(n); } */
            /* } */
        }

        void Run(uint8_t threadCount, uint8_t taskNum) {
            const uint32_t lineStep = static_cast<uint32_t>(threadCount);
            const uint32_t lineOff = static_cast<uint32_t>(taskNum);

            switch (m_bmp->GetFormat().GetImageFormat()) {
                case ImageFormat::Mono16:
                    {
                        uint16_t min = (1 << 16) - 1;
                        uint16_t max = 0; 

                        for (uint32_t y = lineOff; y < m_height; y += lineStep) {
                            const uint16_t* const srcLine = static_cast<const uint16_t*>(m_bmp->GetScanLine((uint16_t)y));
                            minmax<uint16_t>(srcLine, min, max);
                        }
                        {
                            std::unique_lock<std::mutex> lock(m_lock);
                            //m_stats.push_back(std::make_tuple<uint32_t, uint32_t>(static_cast<uint32_t>(min), static_cast<uint32_t>(max)));
                        }
                    }
                    break;
                default:
                    break;
            };

            //m_bmp = nullptr;
            //m_width = 0;
            //m_height = 0;
        };

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
