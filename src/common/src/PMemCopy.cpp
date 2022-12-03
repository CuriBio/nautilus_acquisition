#include <cstring> // std::memcpy

#include <spdlog/spdlog.h>

#include "PMemCopy.h"

PMemCopy::PMemCopy() :
    m_dest(nullptr), m_src(nullptr), m_count(0) {
}

PMemCopy::~PMemCopy() {
}

void PMemCopy::Copy(void* dest, const void* src, size_t count) {
    std::unique_lock<std::mutex> copyLock(m_copyMutex);

    m_dest = dest;
    m_src = src;
    m_count = count;
}

void PMemCopy::Run(uint8_t threadCount, uint8_t taskNum) {
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
