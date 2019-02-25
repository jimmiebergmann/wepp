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

    //inline 

    // Worker implementations.
    template<typename ... Args>
    ThreadWorker<Args...>::ThreadWorker(ThreadPool<Args...> & parent) :
        m_parent(parent),
        m_running(false)
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
    bool ThreadWorker<Args...>::start(std::function<void(Args...)> function)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_running)
        {
            return false;
        }
        m_running = true;

        m_stopTask = Task<>();
        m_function = function;

        m_thread = std::thread([this, function]() mutable
        {
            while (m_running)
            {
                m_executeSemaphore.wait();

                if (!m_running)
                {
                    break;
                }

                callFunction();
            }

            m_stopTask.finish();
        });

        return true;
    }

    template<typename ... Args>
    Task<> ThreadWorker<Args...>::stop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        TaskController<> task;

        if (!m_running)
        {
            return task.finish();
        }
        m_running = false;
        m_executeSemaphore.notifyOne();

        return m_stopTask;
    }

    template<typename ... Args>
    void ThreadWorker<Args...>::execute(std::tuple<Args...> args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_running)
        {
            throw std::runtime_error("Thread job has been stopped.");
        }

        m_parameters = args;

        m_executeSemaphore.notifyOne();

    }

    template<typename ... Args>
    void ThreadWorker<Args...>::callFunction()
    {
        callFunctionHelper(std::index_sequence_for<Args...>());
    }

    template<typename ... Args>
    template<std::size_t... Is>
    void ThreadWorker<Args...>::callFunctionHelper(std::index_sequence<Is...>)
    {
        m_function(std::get<Is>(m_parameters)...);
    }


    // Pool implementations.
    template<typename ... Args>
    ThreadPool<Args...>::ThreadPool() :
        m_running(false),
        m_minWorkers(0),
        m_maxWorkers(0)
    { }

    template<typename ... Args>
    ThreadPool<Args...>::~ThreadPool()
    {
        stop().wait();

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    template<typename ... Args>
    Task<> ThreadPool<Args...>::start(std::function<void(Args...)> function, const size_t minWorkers, const size_t maxWorkers = 0)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        TaskController<> task;

        if (m_running)
        {
            return task.fail();
        }
        m_running = true;

        m_minWorkers = minWorkers;
        m_maxWorkers = m_maxWorkers >= m_minWorkers ? m_maxWorkers : m_minWorkers;
        m_function = function;

        m_stopTask = TaskController<>();

        m_thread = std::thread([this, task]() mutable
        {
            std::tuple<Args...> job;
            ThreadWorker<Args...> * worker = nullptr;

            task.finish();
            while (m_running)
            {
                m_jobSemaphore.wait();
                m_workerSemaphore.wait();
                if (!m_running)
                {
                    break;
                }

                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    job = m_jobQueue.front();
                    m_jobQueue.pop();

                    worker = m_workerQueue.front();
                    m_workerQueue.pop();
                }

                worker->execute(job);
            }

            // Stopping.
            for (auto it = m_workerSet.begin(); it != m_workerSet.end(); it++)
            {
                delete *it;
            }
            m_workerSet.clear();
            while (m_workerQueue.size())
            {
                m_workerQueue.pop();
            }
            m_stopTask.finish();

        });

        allocateWorkers(minWorkers);
        return task;
    }

    template<typename ... Args>
    Task<> ThreadPool<Args...>::stop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_running)
        {
            return m_stopTask;
        }
        m_running = false;
        m_jobSemaphore.notifyOne();
        m_workerSemaphore.notifyOne();

        return m_stopTask;
    }

    template<typename ... Args>
    bool ThreadPool<Args...>::enqueue(Args ... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_running)
        {
            return false;
        }

        m_jobQueue.push(std::tuple<Args...>(args...));
        m_jobSemaphore.notifyOne();

        return true;
    }

    template<typename ... Args>
    void ThreadPool<Args...>::allocateWorkers(const size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            auto worker = new ThreadWorker<Args...>(*this);
            worker->start([this, worker](Args... args)
            {
                m_function(args...);
                enqueueWorker(worker);
            });

            m_workerQueue.push(worker);
            m_workerSet.insert(worker);
            m_workerSemaphore.notifyOne();
        }
    }

    template<typename ... Args>
    void ThreadPool<Args...>::enqueueWorker(ThreadWorker<Args...> * worker)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_workerQueue.push(worker);
        m_workerSemaphore.notifyOne();
    }

}
