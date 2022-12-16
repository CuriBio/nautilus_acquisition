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
#ifndef TASK_FRAMESTATS_H
#define TASK_FRAMESTATS_H

#include <mutex>
#include <vector>
#include <ranges>
#include <stdlib.h>

class TaskFrameStats {
    private:
        std::mutex m_lock;

        uint32_t m_width, m_height;
        uint32_t m_min, m_max;
        uint32_t m_hmax;
        uint32_t* m_hist{nullptr};
        const uint16_t* m_data;

        std::vector<uint32_t*> m_taskHists{};

    public:
        TaskFrameStats(uint8_t numTasks) {
            m_width = m_height = 0;
            m_data = nullptr;

            for (size_t i = 0; i < numTasks; i++) {
                m_taskHists.push_back(new uint32_t[(1<<16)-1]);
            }
        };

        ~TaskFrameStats() {
            for (auto& h : m_taskHists) { delete h; }
        }

        void Setup(const uint16_t* data, uint32_t* hist, uint32_t width, uint32_t height) {
            std::unique_lock<std::mutex> lock(m_lock);
            m_min = (1 << 16) - 1; m_max = 0; m_hmax = 0;

            m_hist = hist;
            memset((void*)m_hist, 0, sizeof(uint32_t)*((1<<16)-1));

            m_width = width; m_height = height;
            m_data = data;
        };

        void Results(uint32_t& min, uint32_t& max, uint32_t& hmax) {
            std::unique_lock<std::mutex> lock(m_lock);
            min = m_min; max = m_max; hmax = m_hmax;
        }

        void Run(uint8_t threadCount, uint8_t taskNum) {
            //uint32_t hist[((1<<16) - 1)] = {0};
            uint32_t* hist = m_taskHists[taskNum];
            memset((void*)hist, 0, sizeof(uint32_t)*((1<<16)-1));

            uint16_t min = (1 << 16) - 1;
            uint16_t max = 0;
            size_t rem = 0;

            if (taskNum == threadCount - 1) {
                rem = m_height % threadCount;
            }

            const size_t rows = rem + (m_height / threadCount);
            const size_t rowOffset = taskNum * rows;
            const size_t offset = rowOffset * m_width;

            const uint16_t* start = m_data+offset;
            const uint16_t* end = m_data+offset+(rows*m_width);

            for (; start < end; start++) {
                if (*start > max) { max = *start; }
                if (*start < min) { min = *start; }

                hist[*start] += 1;
            }

            {
                std::unique_lock<std::mutex> lock(m_lock);
                if (min < m_min) { m_min = min; }
                if (max > m_max) { m_max = max; }

                for (size_t i=0; i < ((1<<16) - 1); i++) {
                    m_hist[i] += hist[i];
                    if (m_hist[i] > m_hmax) { m_hmax = m_hist[i]; }
                }
            }
        }
};
#endif //TASK_FRAMESTATS_H
