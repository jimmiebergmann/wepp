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
            m_state(State::Stopped),
            m_pool(pool)
        { }

        template<typename ... Args>
        ThreadWorker<Args...>::~ThreadWorker()
        {
            stop().wait();
            if (m_thread.joinable())
            {
                m_thread.join();
            }
        }

        template<typename ... Args>
        void ThreadWorker<Args...>::enqueue(Args ... args)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_args.push(std::tuple<Args...>(args...));
            m_semaphore.notifyOne();
        }

        template<typename ... Args>
        Task<> ThreadWorker<Args...>::start()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (m_state != State::Stopped)
            {
                return TaskController<>().fail();
            }

            m_state = State::Starting;
            m_stopTask = TaskController<>();
            m_startTask = TaskController<>();

            if (m_thread.joinable())
            {
                m_thread.join();
            }

            m_thread = std::thread([this]()
            {
                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    if (m_state == State::Starting)
                    {
                        m_state = State::Started;
                    }

                    m_startTask.finish();
                }

                while (m_state == State::Started)
                {
                    m_semaphore.wait();

                    if (m_state != State::Started)
                    {
                        break;
                    }

                    executeVirtual();

                    // Return worker to parent pool if no more work is available.
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);

                        if (!m_args.size())
                        {
                            m_pool->returnWorker(this);
                        }
                    }
                   
                }

                handleStop();

            });
            return m_startTask;
        }

        template<typename ... Args>
        Task<> ThreadWorker<Args...>::stop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (m_state == State::Stopping)
            {
                return m_stopTask;
            }
            else if (m_state == State::Stopped)
            {
                return m_stopTask.finish();
            }

            m_state = State::Stopping;
            m_semaphore.notifyOne();

            return m_stopTask;
        }

        template<typename ... Args>
        void ThreadWorker<Args...>::handleStop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_state = State::Stopped;
            m_stopTask.finish();
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
            std::tuple<Args...> * args = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                args = &m_args.front();
            }

            execute(std::get<Is>(*args)...);

            {
                std::lock_guard<std::mutex> lock(m_mutex);

                m_args.pop();
            }
        }


        // ThreadPool implementations.
        template<typename T, typename ... Args>
        ThreadPool<T, Args...>::ThreadPool() :
            m_state(State::Stopped),
            m_minWorkers(0),
            m_maxWorkers(0)
        { }

        template<typename T, typename ... Args>
        ThreadPool<T, Args...>::~ThreadPool()
        {
            stop().wait();
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

            if (m_state != State::Stopped)
            {
                return TaskController<>().fail();
            }

            if (!minWorkers)
            {
                throw std::runtime_error("Starting thread pool with 0 as minimum numbers of workers.");
            }

            m_minWorkers = minWorkers;
            m_maxWorkers = maxWorkers >= minWorkers ? maxWorkers : minWorkers;

            m_state = State::Starting;
            m_stopTask = TaskController<>();
            m_startTask = TaskController<>();

            m_enqueueSemaphore.reset();
            m_availableSemaphore.reset();

            if (m_thread.joinable())
            {
                m_thread.join();
            }

            // Start main thread.
            m_thread = std::thread([this, cargs...]()
            {
                // Allocate workers.
                WEPP_DO_ONCE
                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    if (m_state != State::Starting)
                    {
                        break;
                    }

                    std::vector<Task<> > workerStartTasks;
                    workerStartTasks.reserve(m_minWorkers);

                    for (size_t i = 0; i < m_minWorkers; i++)
                    {
                        T * worker = new T(this, cargs...);
                        m_workerSet.insert(worker);
                        m_workerQueue.push(worker);
                        workerStartTasks.push_back(worker->start());
                        m_availableSemaphore.notifyOne();
                    }

                    // Wait for workers to start.
                    for (auto it = workerStartTasks.begin(); it != workerStartTasks.end(); it++)
                    {
                        (*it).wait();
                    }
                }

                // Set task as finished.
                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    if (m_state == State::Starting)
                    {
                        m_state = State::Started;
                    }
                    m_startTask.finish();
                }

                // Distribute work / allocate new workers.
                while (m_state == State::Started)
                {
                    m_enqueueSemaphore.wait();
                    m_availableSemaphore.wait();
                    
                    if (m_state != State::Started)
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

                // Finishing.
                handleStop();
                
            });

            return m_startTask;
        }

        template<typename T, typename ... Args>
        Task<> ThreadPool<T, Args...>::stop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (m_state == State::Stopping)
            {
                return m_stopTask;
            }
            else if (m_state == State::Stopped)
            {
                return m_stopTask.finish();
            }

            m_state = State::Stopping;

            m_enqueueSemaphore.notifyOne();     // Free main loop from blockage.
            m_availableSemaphore.notifyOne();   // Free main loop from blockage.

            return m_stopTask;
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
        void ThreadPool<T, Args...>::handleStop()
        {
            // Temporary store workers.
            std::vector<Worker *> workers;
            std::vector<Task<> > workerStopTasks;
            workers.reserve(m_workerSet.size());
            workerStopTasks.reserve(m_workerSet.size());

            // Temporary store workers.
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                for (auto it = m_workerSet.begin(); it != m_workerSet.end(); it++)
                {
                    workers.push_back(*it);
                }
            }

            // Stop workers.
            for (auto it = workers.begin(); it != workers.end(); it++)
            {
                workerStopTasks.push_back((*it)->stop());
            }


            // Wait for workers to stop.
            for (auto it = workerStopTasks.begin(); it != workerStopTasks.end(); it++)
            {
                it->wait();
            }

            // Cleanup workers.
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                for (auto it = m_workerSet.begin(); it != m_workerSet.end(); it++)
                {
                    delete *it;
                }

                while (m_workerQueue.size())
                {
                    m_workerQueue.pop();
                }
            }

            m_state = State::Stopped;
            m_stopTask.finish();
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
            worker->enqueue(std::get<Is>(work)...);
        }

    }

}
