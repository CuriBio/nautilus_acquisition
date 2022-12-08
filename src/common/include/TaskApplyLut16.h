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
