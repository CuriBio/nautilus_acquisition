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
#include <PMemCopy.h>

template<FrameConcept F>
class FramePool {
    private:
        std::queue<F*> m_pool;
        std::mutex m_poolLock;
        //std::tuple<Args...> m_params;
        std::shared_ptr<PMemCopy> m_pCopy;
        size_t m_frameBytes;
        bool m_deepCopy;
        size_t m_size;

        size_t total_objs{0};

    public:
        FramePool(size_t poolSize, size_t frameBytes, bool deepCopy, std::shared_ptr<PMemCopy> pCopy) {
            m_frameBytes = frameBytes;
            m_deepCopy = deepCopy;
            m_pCopy = pCopy;

            total_objs += poolSize;

            for(size_t i = 0; i < poolSize; i++) {
                m_pool.push(new F(m_frameBytes, m_deepCopy, m_pCopy));
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
                obj = new F(m_frameBytes, m_deepCopy, m_pCopy);
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
                m_pool.push(new F(m_frameBytes, m_deepCopy, m_pCopy));
                total_objs++;
            }
        }

        void Release(F* obj) noexcept {
            std::lock_guard<std::mutex> lock(m_poolLock);
            m_pool.push(obj);
        }
};

#endif //FRAME_POOL_H
