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
 * @file  ThreadPool.h
 * 
 * Definition of ThreadPool class.
 *********************************************************************/
#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

/**
 * @brief A convenient shorthand for the type of std::thread::hardware_concurrency(). Should evaluate to unsigned int.
 */
using concurrency_t = std::invoke_result_t<decltype(std::thread::hardware_concurrency)>;

class ThreadPool {
    private:
        /** @brief The total number of threads managed by pool */
        concurrency_t m_threadCount{0};

        /** @brief A smart pointer to manage the memory allocated for the threads. */
        std::unique_ptr<std::thread[]> m_threads{nullptr};

        /** @brief Task mutex */
        mutable std::mutex m_tasksMutex{};

        /** @brief Finished tasks mutex */
        mutable std::mutex m_tasksFinishedMutex{};

        /** @brief Finished tasks mutex */
        mutable std::mutex m_queueMutex{};

        /** @brief Task counter */
        std::atomic<size_t> m_totalTasks{0};

        /** @brief Task ready condition variable */
        std::condition_variable m_taskReady{};

        /** @brief Task finished condition variable */
        std::condition_variable m_tasksFinished{};

        /** @brief Task queue */
        std::queue<std::function<void()>> m_tasks{};

        /** @brief Atomic flag to control stopping worker threads */
        std::atomic<bool> m_running{false};

    public:
        /*
        * @brief Constructor
        *
        * @param threads Number of threads to run the executor.
        */
        ThreadPool(const concurrency_t threadCount = 0) : 
            m_threadCount(determineThreadCount(threadCount)),
            m_threads(std::make_unique<std::thread[]>(determineThreadCount(threadCount)))
        {
            initThreads();
        }

        /** @brief Destructor */
        ~ThreadPool() {
            WaitForAll();
            destroyThreads();
        }

        /** @brief Add task with no return value */
        template <typename F, typename... A>
        void AddTask(F&& fn, A&&... args) noexcept {
            {
                std::unique_lock<std::mutex> _lock(m_tasksMutex);
                std::function<void()> task = std::bind(std::forward<F>(fn), std::forward<A>(args)...);
                m_tasks.push(task);
                ++m_totalTasks;
                m_taskReady.notify_one();
            }
        }

        /** @brief Wait for all running tasks */
        void WaitForAll() noexcept {
            std::unique_lock<std::mutex> _lock(m_tasksFinishedMutex);
            if (m_totalTasks > 0 ) {
                m_tasksFinished.wait(_lock, [this]() { return m_totalTasks == 0; });
            }
        }

        size_t Size() noexcept {
            std::unique_lock<std::mutex> _lock(m_tasksMutex);
            return m_tasks.size();
        }

        concurrency_t ThreadCount() const {
            return m_threadCount;
        }

    private:
        /**
         * @brief Determine supported hardware concurrency.
         *
         * @param count The desired number of threads, if 0 use max hardware concurrency
         **/
        concurrency_t determineThreadCount(const concurrency_t count) {
            if (count > 0) {
                return count;
            } else if (std::thread::hardware_concurrency() > 0) {
                return std::thread::hardware_concurrency();
            } else {
                return 1;
            }
        }

        /** @brief Initial threads */
        void initThreads() {
            for (concurrency_t c = 0; c < m_threadCount; ++c) {
                m_threads[c] = std::thread(&ThreadPool::worker, this, c+1);
            }
            m_running = true;
        }

        /** @brief destroy threads */
        void destroyThreads() {
            m_running = false;
            m_taskReady.notify_all();

            for (concurrency_t i = 0; i < m_threadCount; ++i) {
                m_threads[i].join();
            }
        }

        /** @brief Thread worker loop */
        void worker(size_t workerId) noexcept {
            std::function<void()> task = {};

            std::unique_lock<std::mutex> lock(m_tasksMutex);
            while (m_running) {
                m_taskReady.wait(lock, [&]() { return !m_tasks.empty() || !m_running; });

                if (!m_tasks.empty()) {
                    task = std::move(m_tasks.front());
                    m_tasks.pop();

                    lock.unlock();
                    task();
                    lock.lock();
                    if (--m_totalTasks == 0) { m_tasksFinished.notify_one(); }
                }
            }
        }
};

#endif //__THREAD_POOL_H
