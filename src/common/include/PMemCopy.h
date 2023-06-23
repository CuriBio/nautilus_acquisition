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
 * @file  PMemCopy.h
 * 
 * Definition of the PMemCopy class.
 *********************************************************************/
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <memory> // std::unique_ptr
#include <mutex>
#include <thread>
#include <vector>

#include <spdlog/spdlog.h>


/*
* Parallel memory copy.
*/
class PMemCopy {
    private:
        std::mutex m_copyMutex{};

        void* m_dest{nullptr};
        const void* m_src{nullptr};
        size_t m_count;

    public:
        /*
         * Parallel memory class constructor.
         */
        PMemCopy() : m_dest(nullptr), m_src(nullptr), m_count(0) {
        }

        /*
         * Parallel memory class destructor.
         */
        ~PMemCopy() {
        }

        /*
         * Setup parallel copy task.
         *
         * @param dest The destination memory location.
         * @param src The source memory location.
         * @param count The number of bytes to copy.
         */
        void Copy(void* dest, const void* src, size_t count) {
            std::unique_lock<std::mutex> copyLock(m_copyMutex);

            m_dest = dest;
            m_src = src;
            m_count = count;
        }

        /*
         * Run task.
         *
         * @param threadCount The number of threads running in parallel.
         * @param taskNum The id of this task.
         */
        void Run(uint8_t threadCount, uint8_t taskNum) {
            size_t rem = 0;

            size_t bytes = m_count / threadCount;
            const size_t offset = taskNum * bytes;

            if (taskNum == threadCount - 1) {
                rem = m_count % threadCount;
            }

            void* dst = static_cast<uint8_t*>(m_dest) + offset;
            const void* src = static_cast<const uint8_t*>(m_src) + offset;

            std::memcpy(dst, src, bytes + rem);
        }
};

#endif //THREAD_POOL_H
