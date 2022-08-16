#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H
#include <memory>
#include <mutex>
#include <queue>

#include <spdlog/spdlog.h>

template<typename T>
class ObjPool {
    private:
        std::queue<std::shared_ptr<T>> m_pool;
        std::mutex m_poolLock;

    public:
        template<typename... Rest>
        ObjPool(size_t size, Rest const& ... rest) {
            spdlog::info("Initializing frame pool with {} objects", size);
            for(size_t i = 0; i < size; i++) {
                m_pool.push(std::make_shared<T>(rest...));
            }
        }

        ~ObjPool() = default;

        std::shared_ptr<T> Acquire() noexcept {
            std::lock_guard<std::mutex> lock(m_poolLock);

            if (!m_pool.empty()) {
                std::shared_ptr<T> obj = std::move(m_pool.front());
                m_pool.pop();
                return obj;
            }
            return nullptr;
        }

        void Release(std::shared_ptr<T> obj) noexcept {
            std::lock_guard<std::mutex> lock(m_poolLock);
            m_pool.push(std::move(obj));
        }
};

#endif //OBJECT_POOL_H
