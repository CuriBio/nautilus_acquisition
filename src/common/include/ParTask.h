#ifndef PAR_TASK_H
#define PAR_TASK_H

#include <condition_variable>
#include <memory> // std::unique_ptr
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <functional>
#include <barrier>

#include <spdlog/spdlog.h>
#include <interfaces/ParTaskInterface.h>

class ParTask {
    private:
        static inline uint32_t taskCount;

        static inline std::mutex m_taskLock;
        //std::shared_mutex m_updateLock;
        std::mutex m_queueMutex;

        std::mutex m_waitMutex;
        std::mutex m_lock;
        std::condition_variable m_mutexCond;
        std::condition_variable m_waitCond;

        std::vector<std::thread> m_threads;

        /* std::function<void()> on_complete() { */
        /*     m_running = false; */
        /*     m_waitCond.notify_one(); */
        /* }; */

        /* std::shared_ptr<std::barrier<decltype(ParTask::on_complete)>> m_barrier; */

        uint8_t m_threadCount{0};
        uint8_t m_running{0};

        bool m_stopFlag{false};

        std::function<void(uint8_t, uint8_t)> m_task;

    public:
        ParTask(uint8_t threads) : m_threadCount(threads) {
            m_threads.reserve(m_threadCount);
            m_running = 0;
            /* m_barrier = std::make_shared<std::barrier<decltype(on_complete)>>(m_threadCount, on_complete); */

            spdlog::info("Starting {} threads", m_threadCount);

            for (uint8_t n = 0; n < m_threadCount; ++n) {
                auto thread = std::thread(&ParTask::executor, this, n);
                m_threads.push_back(std::move(thread));
            }
        }

        ~ParTask() {
            m_stopFlag = true;
            m_mutexCond.notify_all();

            for (auto& t : m_threads) {
                t.join();
            }
        }

        template<TaskConcept T>
        void Start(std::shared_ptr<T> task) {
            std::unique_lock<std::mutex> lock(m_taskLock);
            m_task = std::bind(&T::Run, task, std::placeholders::_1, std::placeholders::_2);
            start();
        }

    private:
        void start() {
            m_running = m_threadCount;
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_mutexCond.notify_all();
            }

            std::unique_lock<std::mutex> lock(m_waitMutex);
            m_waitCond.wait(lock, [&]() { return (m_running == 0); });
        }

        void executor(uint8_t taskNum) {
            size_t rem = 0;

            while (!m_stopFlag) {
                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    m_mutexCond.wait(lock);

                    if (m_stopFlag) {
                        break;
                    }
                }

                m_task(m_threadCount, taskNum);
                {
                    std::unique_lock<std::mutex> update(m_lock);
                    m_running--;

                    if (m_running == 0) { m_waitCond.notify_one(); }
                }
            }
        }
};

#endif //THREAD_POOL_H
