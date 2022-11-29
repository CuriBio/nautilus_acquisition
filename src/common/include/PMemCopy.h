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
        uint8_t m_threadCount;

        std::mutex m_copyMutex;
        std::mutex m_queueMutex;
        std::condition_variable m_mutexCond;
        std::mutex m_waitMutex;
        std::condition_variable m_waitCond;

        std::vector<std::thread> m_threads;
        bool m_stopFlag{false};

        void* m_dest{nullptr};
        const void* m_src{nullptr};
        size_t m_count;

        std::atomic_uint8_t m_runningCount;

    public:
        PMemCopy(uint8_t threads);
        ~PMemCopy();

        void Copy(void* dest, const void* src, size_t count);

    private:
        void threadFunc(uint8_t taskNum);
};

#endif //THREAD_POOL_H
