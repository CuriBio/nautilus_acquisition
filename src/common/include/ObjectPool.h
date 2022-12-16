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
#ifndef FRAME_POOL_H
#define FRAME_POOL_H
#include <memory>
#include <mutex>
#include <queue>
#include <functional>
#include <utility>
#include <tuple>

#include <spdlog/spdlog.h>
#include <interfaces/FrameInterface.h>
#include <ParTask.h>

template<FrameConcept F>
class FramePool {
    private:
        std::queue<F*> m_pool;
        std::mutex m_poolLock;
        //std::tuple<Args...> m_params;
        std::shared_ptr<ParTask> m_pTask;
        size_t m_frameBytes;
        bool m_deepCopy;
        size_t m_size;

        size_t total_objs{0};

    public:
        FramePool(size_t poolSize, size_t frameBytes, bool deepCopy, std::shared_ptr<ParTask> pTask) {
            m_frameBytes = frameBytes;
            m_deepCopy = deepCopy;
            m_pTask = pTask;

            total_objs += poolSize;

            for(size_t i = 0; i < poolSize; i++) {
                m_pool.push(new F(m_frameBytes, m_deepCopy, m_pTask));
            }
            spdlog::info("Pool size: {}", total_objs);
        }

        ~FramePool() = default;

        F* Acquire() noexcept {
            std::lock_guard<std::mutex> lock(m_poolLock);
            F* obj;

            if (!m_pool.empty()) {
                obj = m_pool.front();
                m_pool.pop();
            } else {
                spdlog::info("Pool empty, allocating");
                obj = new F(m_frameBytes, m_deepCopy, m_pTask);
            }
            if (!obj) {
                return nullptr;
            }

            return obj;
            /* return std::shared_ptr<F>(obj.release(), */
            /*         std::bind(&FramePool::Release, this, std::placeholders::_1)); */
        }

        size_t Size() {
            std::lock_guard<std::mutex> lock(m_poolLock);
            return m_pool.size();
        }

        void EnsurePoolSize(uint16_t size) {
            std::lock_guard<std::mutex> lock(m_poolLock);

            while (m_pool.size() < size) {
                spdlog::info("Pool low, allocating");
                m_pool.push(new F(m_frameBytes, m_deepCopy, m_pTask));
                total_objs++;
            }
        }

        void Release(F* obj) noexcept {
            std::lock_guard<std::mutex> lock(m_poolLock);
            m_pool.push(obj);
        }
};

#endif //FRAME_POOL_H
