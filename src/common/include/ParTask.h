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

/*********************************************************************
 * @file  ParTask.h
 * 
 * Definition of the ParTask class.
 *********************************************************************/
#ifndef PAR_TASK_H
#define PAR_TASK_H

#include <condition_variable>
#include <memory> // std::unique_ptr
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <functional>

#include <spdlog/spdlog.h>
#include <interfaces/ParTaskInterface.h>

/*
* Parallel task executor class.
*/
class ParTask {
    private:
        static inline uint32_t taskCount;

        static inline std::mutex m_taskLock;
        std::mutex m_queueMutex;

        std::mutex m_waitMutex;
        std::mutex m_lock;
        std::condition_variable m_mutexCond;
        std::condition_variable m_waitCond;

        std::vector<std::thread> m_threads;

        uint8_t m_threadCount{0};
        uint8_t m_running{0};

        bool m_stopFlag{false};
        std::function<void(uint8_t, uint8_t)> m_task;

    public:
        /*
        * Parallel task executor class constructor.
        *
        * @param threads Number of threads to run the executor.
        */
        ParTask(uint8_t threads) : m_threadCount(threads) {
            m_threads.reserve(m_threadCount);
            m_running = 0;

            spdlog::info("Starting {} threads", m_threadCount);

            for (uint8_t n = 0; n < m_threadCount; ++n) {
                auto thread = std::thread(&ParTask::executor, this, n);
                m_threads.push_back(std::move(thread));
            }
        }

        /*
         * Parallel task executor destructor.
         */
        ~ParTask() {
            m_stopFlag = true;
            m_mutexCond.notify_all();

            for (auto& t : m_threads) {
                t.join();
            }
        }

        /*
         * Start task.
         *
         * @tparam T Task concept type.
         * @param task Pointer to the task object.
         */
        template<TaskConcept T>
        void Start(std::shared_ptr<T> task) {
            std::unique_lock<std::mutex> lock(m_taskLock);
            m_task = std::bind(&T::Run, task, std::placeholders::_1, std::placeholders::_2);
            start();
        }

    private:
        /*
         * Starts task on all threads.
         */
        void start() {
            m_running = m_threadCount;
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_mutexCond.notify_all();
            }

            std::unique_lock<std::mutex> lock(m_waitMutex);
            m_waitCond.wait(lock, [&]() { return (m_running == 0); });
        }

        /*
         * Executor thread function.
         *
         * @param taskNum The thread id.
         */
        void executor(uint8_t taskNum) {
            size_t rem = 0;

            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_mutexCond.wait(lock);
            }

            while (!m_stopFlag) {
                //run task
                m_task(m_threadCount, taskNum);
                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    m_running--;

                    if (m_running == 0) { m_waitCond.notify_one(); }
                    m_mutexCond.wait(lock);
                }
            }
        }
};

#endif //THREAD_POOL_H
