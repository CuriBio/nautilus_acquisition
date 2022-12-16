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
#ifndef TASK_FRAMELUT16_H
#define TASK_FRAMELUT16_H

#include <mutex>
#include <vector>
#include <ranges>

class TaskFrameLut16 {
    private:
        std::mutex m_lock;

        uint16_t m_min, m_max;
        uint8_t m_lut16[((1<<16) - 1)] = {0};
        double m_scale;

    public:
        TaskFrameLut16() { };

        ~TaskFrameLut16() = default;

        void Setup(uint32_t min, uint32_t max) {
            std::unique_lock<std::mutex> lock(m_lock);
            m_min = min; m_max = max;
            m_scale = (max == min) ? 255.0 : 255.0 / static_cast<double>(max-min);
        };

        uint8_t* Results() {
            std::unique_lock<std::mutex> lock(m_lock);
            return &m_lut16[0];
        }

        void Run(uint8_t threadCount, uint8_t taskNum) {
            size_t rem = 0;
            size_t chunkSize = ((1<<16) - 1) / threadCount;

            if (taskNum == threadCount - 1) {
                rem = ((1<<16) - 1) % threadCount;
            }

            size_t start = taskNum * chunkSize;
            size_t end = start + chunkSize+rem;

            for (size_t i = start; i <= end; i++) {
                if (i < m_min) {
                    m_lut16[i] = 0;
                } else if (i > m_max) {
                    m_lut16[i] = 255;
                } else {
                    double value = static_cast<double>(m_scale * (i - m_min));
                    m_lut16[i] = static_cast<uint8_t>(value);
                }
            }
        }
};
#endif //TASK_FRAMELUT16_H
