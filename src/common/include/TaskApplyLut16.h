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
#ifndef TASK_APPLYLUT16_H
#define TASK_APPLYLUT16_H

#include <mutex>
#include <vector>
#include <ranges>

class TaskApplyLut16 {
    private:
        std::mutex m_lock;
        const uint16_t* m_data;
        const uint8_t* m_lut;
        uint8_t* m_out;
        size_t m_size;

    public:
        TaskApplyLut16() { };

        ~TaskApplyLut16() = default;

        void Setup(const uint16_t* data, uint8_t* out, const uint8_t* lut, size_t size) {
            std::unique_lock<std::mutex> lock(m_lock);
            m_data = data;
            m_out = out;
            m_lut = lut;
            m_size = size;
        };

        uint8_t* Results() {
            std::unique_lock<std::mutex> lock(m_lock);
            return &m_out[0];
        }

        void Run(uint8_t threadCount, uint8_t taskNum) {
            size_t rem = 0;
            size_t chunkSize = m_size / threadCount;

            if (taskNum == threadCount - 1) {
                rem = m_size % threadCount;
            }

            size_t start = taskNum * chunkSize;
            size_t end = start + chunkSize + rem;

            for (size_t i = start; i < end; i++) {
                m_out[i] = m_lut[m_data[i]];
            }
        }
};
#endif //TASK_APPLYLUT16_H
