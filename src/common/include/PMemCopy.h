#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <memory> // std::unique_ptr
#include <mutex>
#include <thread>
#include <vector>

class PMemCopy {
    private:
        std::mutex m_copyMutex{};

        void* m_dest{nullptr};
        const void* m_src{nullptr};
        size_t m_count;

    public:
        PMemCopy();
        ~PMemCopy();

        void Copy(void* dest, const void* src, size_t count);
        void Run(uint8_t threadCount, uint8_t taskNum);
};

#endif //THREAD_POOL_H
