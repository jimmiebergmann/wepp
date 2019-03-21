/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

namespace Wepp
{

    namespace Priv
    {


        // ThreadWorker implementations.
        template<typename ... Args>
        ThreadWorker<Args...>::ThreadWorker(ThreadPoolBase<Args...> * pool) :
            m_running(false),
            m_pool(pool)
        {
            Semaphore semaphore;
            m_thread = std::thread([this, &semaphore]()
            {
                m_running = true;
                semaphore.notifyOne();

                while (m_running)
                {
                    m_semaphore.wait();
                    if (!m_running)
                    {
                        break;
                    }

                    executeVirtual();
                }

            });
            semaphore.wait();
        }

        template<typename ... Args>
        ThreadWorker<Args...>::~ThreadWorker()
        {
            m_running = false;
            m_semaphore.notifyOne();
            if (m_thread.joinable())
            {
                m_thread.join();
            }
        }

        template<typename ... Args>
        void ThreadWorker<Args...>::work(Args ... args)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_args = std::tuple<Args...>(args...);
            m_semaphore.notifyOne();
        }

        template<typename ... Args>
        void ThreadWorker<Args...>::executeVirtual()
        {
            executeVirtualHelper(std::index_sequence_for<Args...>());
        }

        template<typename ... Args>
        template<std::size_t... Is>
        void ThreadWorker<Args...>::executeVirtualHelper(std::index_sequence<Is...>)
        {
            execute(std::get<Is>(m_args)...);
            m_pool->returnWorker(this);
        }

        // ThreadPool implementations.
        template<typename T, typename ... Args>
        ThreadPool<T, Args...>::ThreadPool() :
            m_started(false),
            m_stopped(true),
            m_minWorkers(0),
            m_maxWorkers(0)
        { }

        template<typename T, typename ... Args>
        ThreadPool<T, Args...>::~ThreadPool()
        {
            if (stop().wait().failed())
            {
                //  throw std::runtime_error("Failed to stop thread pool at destruction.");
            }

            if (m_thread.joinable())
            {
                m_thread.join();
            }
        }

        template<typename T, typename ... Args>
        template<typename ... CArgs>
        Task<> ThreadPool<T, Args...>::start(const size_t minWorkers, const size_t maxWorkers, CArgs ... cargs)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (m_started || !m_stopped)
            {
                return TaskController<>().fail();
            }
            m_startedTask = TaskController<>();

            if (!minWorkers)
            {
                throw std::runtime_error("Starting thread pool with 0 as minimum numbers of workers.");
            }

            m_minWorkers = minWorkers;
            m_maxWorkers = maxWorkers >= minWorkers ? maxWorkers : minWorkers;

            m_enqueueSemaphore.reset();
            m_availableSemaphore.reset();

            // Start thread. Notify wait semaphore when thread is started, to exit this function.
            Semaphore threadSemaphore;
            m_thread = std::thread([this, cargs..., &threadSemaphore]()
            {
                m_started = true;
                m_stopped = false;
                threadSemaphore.notifyOne();

                // Allocate workers.
                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    for (size_t i = 0; i < m_minWorkers; i++)
                    {
                        T * worker = new T(this, cargs...);
                        m_workerSet.insert(worker);
                        m_workerQueue.push(worker);
                        m_availableSemaphore.notifyOne();
                    }
                }

                // The thread pool has been started, mark as finished.
                m_startedTask.finish();

                // Distribute work / allocate new workers.
                while (m_started)
                {
                    m_enqueueSemaphore.wait();
                    m_availableSemaphore.wait();
                    if (!m_started)
                    {
                        break;
                    }

                    // Execute work.
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);
                        auto worker = m_workerQueue.front();
                        m_workerQueue.pop();

                        Work work = m_workQueue.front();
                        m_workQueue.pop();

                        executeWorker(worker, work);
                    }
                }

                cleanup();
            });

            threadSemaphore.wait();
            return m_startedTask;
        }

        template<typename T, typename ... Args>
        Task<> ThreadPool<T, Args...>::stop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (!m_started || m_stopped)
            {
                return TaskController<>().finish();
            }
            m_stoppedTask = TaskController<>();

            m_started = false;
            m_enqueueSemaphore.notifyOne();     // Free main loop from blockage.
            m_availableSemaphore.notifyOne();  // Free main loop from blockage.

            return m_stoppedTask;
        }

        template<typename T, typename ... Args>
        void ThreadPool<T, Args...>::enqueue(Args ... args)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_workQueue.push(std::tuple<Args...>(args...));
            m_enqueueSemaphore.notifyOne();
        }

        template<typename T, typename ... Args>
        inline void ThreadPool<T, Args...>::returnWorker(Worker * worker)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (m_workerSet.find(worker) == m_workerSet.end())
            {
                throw std::runtime_error("Trying to return worker to a thread pool not owning the worker.");
            }

            m_workerQueue.push(worker);
            m_availableSemaphore.notifyOne();
        }

        template<typename T, typename ... Args>
        void ThreadPool<T, Args...>::cleanup()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            for (auto it = m_workerSet.begin(); it != m_workerSet.end(); it++)
            {
                delete *it;
            }
            m_workerSet.clear();

            while (m_workerQueue.size())
            {
                m_workerQueue.pop();
            }

            while (m_workQueue.size())
            {
                m_workQueue.pop();
            }

            m_stopped = true;
            m_stoppedTask.finish();
        }


        template<typename T, typename ... Args>
        void ThreadPool<T, Args...>::executeWorker(Worker * worker, Work & work)
        {
            executeWorkerHelper(worker, work, std::index_sequence_for<Args...>());
        }

        template<typename T, typename ... Args>
        template<std::size_t... Is>
        void ThreadPool<T, Args...>::executeWorkerHelper(Worker * worker, Work & work, std::index_sequence<Is...>)
        {
            worker->work(std::get<Is>(work)...);
        }

    }

}
