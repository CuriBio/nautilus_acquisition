#include <cstring> // std::memcpy

#include <spdlog/spdlog.h>

#include "PMemCopy.h"

PMemCopy::PMemCopy(uint8_t threads) : m_threadCount(threads) {
    m_threads.reserve(threads);
    m_runningCount = threads;
    spdlog::info("Starting {} threads for PMemCopy", m_runningCount);

    for (uint8_t n = 0; n < threads; ++n) {
        auto thread = std::thread(&PMemCopy::threadFunc, this, n);
        m_threads.push_back(std::move(thread));
    }
}

PMemCopy::~PMemCopy() {
    m_stopFlag = true;
    m_mutexCond.notify_all();

    for (auto& t : m_threads) {
        t.join();
    }
}

void PMemCopy::Copy(void* dest, const void* src, size_t count) {
    std::unique_lock<std::mutex> copyLock(m_copyMutex);

    m_dest = dest;
    m_src = src;
    m_count = count;
    m_runningCount = m_threadCount;

    m_mutexCond.notify_all();
    std::unique_lock<std::mutex> lock(m_waitMutex);
    m_waitCond.wait(lock, [&]() { return (m_runningCount == 0); });
}

void PMemCopy::threadFunc(uint8_t taskNum) {
    size_t rem = 0;

    while (!m_stopFlag) {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_mutexCond.wait(lock);

            if (m_stopFlag) {
                break;
            }
        }

        size_t bytes = m_count / m_threadCount;
        const size_t offset = taskNum * bytes;

        if (taskNum == m_threadCount - 1) {
            rem = m_count % m_threadCount;
        }

        void* dst = static_cast<uint8_t*>(m_dest) + offset;
        const void* src = static_cast<const uint8_t*>(m_src) + offset;

        std::memcpy(dst, src, bytes + rem);

        {
            std::unique_lock<std::mutex> lock(m_waitMutex);
            m_runningCount--;
        }
        m_waitCond.notify_one();
    }
}

