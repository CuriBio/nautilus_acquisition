#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H
#include <memory>
#include <mutex>
#include <queue>
#include <functional>
#include <utility>
#include <tuple>


#include <spdlog/spdlog.h>

template<typename T, typename... Args>
class ObjPool {
    private:
        std::queue<std::shared_ptr<T>> m_pool;
        std::mutex m_poolLock;
        std::tuple<Args...> m_params;
        uint16_t total_objs{0};

    public:
        ObjPool(size_t size, Args const&... rest) {
            spdlog::info("Initializing frame pool with {} objects", size);
            m_params = std::make_tuple(rest...);
            total_objs += size;

            for(size_t i = 0; i < size; i++) {
                m_pool.push(std::make_shared<T>(rest...));
            }
        }

        ~ObjPool() = default;

        std::shared_ptr<T> Acquire() noexcept {
            std::lock_guard<std::mutex> lock(m_poolLock);

            if (!m_pool.empty()) {
                std::shared_ptr<T> obj = m_pool.front();
                m_pool.pop();
                return obj;
            } else {
                std::shared_ptr<T> obj = std::apply(std::make_shared<T, std::add_lvalue_reference_t<Args>...>, m_params);
                return obj;
            }
            return nullptr;
        }

        size_t Size() {
            return m_pool.size();
        }

        void Release(std::shared_ptr<T> obj) noexcept {
            std::lock_guard<std::mutex> lock(m_poolLock);
            m_pool.push(obj);
        }

        void EnsurePoolSize(uint16_t size) {
            std::lock_guard<std::mutex> lock(m_poolLock);

            while (m_pool.size() < size) {
                auto obj = std::apply(std::make_shared<T, std::add_lvalue_reference_t<Args>...>, m_params);
                m_pool.push(obj);
                total_objs++;
            }
        }
};

#endif //OBJECT_POOL_H
