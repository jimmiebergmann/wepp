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

#ifndef WEPP_THREAD_POOL_HPP
#define WEPP_THREAD_POOL_HPP

#include "wepp/build.hpp"
#include "wepp/task.hpp"
#include "wepp/semaphore.hpp"
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <set>
#include <functional>
#include <exception>
#include <tuple>


/**
* Wepp namespace.
*
*/
namespace Wepp
{
    template<typename ... Args>
    class ThreadPool;

    template<typename ... Args>
    class ThreadJob
    {

    public:

        ThreadJob(ThreadPool<Args...> & parent, std::function<void(Args...)> function) :
            m_parent(parent),
            m_running(false),
            m_stopped(true)
        {
            m_running = true;
            m_thread = std::thread([this, function]() mutable
            {
                while (m_running)
                {
                    m_workSemaphore.wait();

                    if (!m_running)
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);
    
                        while (m_stopQueue.size())
                        {
                            m_stopQueue.front().finish();
                            m_stopQueue.pop();
                        }

                        return;
                    }

                    callFunction(function);
                    m_parent.addToQueue(this);

                }

                m_stopped = true;

            });

            m_stopped = false;
        }

        virtual ~ThreadJob()
        {
            if (m_thread.joinable())
            {
                m_thread.join();
            }
        }

        void work(Args ... args)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (!m_running)
            {
                throw std::runtime_error("Thread job has been stopped.");
            }

            m_parameters = std::tuple<Args...>(args...);

            m_workSemaphore.notifyOne();
            
        }

        Task<> stop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            TaskController<> task;

            if (m_stopped)
            {
                return task.successful();
            }

            m_stopQueue.push(task);
            m_running = false;
            m_workSemaphore.notifyOne();

            return task;
        }

    private:

        void callFunction(std::function<void(Args...)> & function)
        {
            callFunctionHelper(function, std::index_sequence_for<Args...>());
        }

        template<std::size_t... Is>
        void callFunctionHelper(std::function<void(Args...)> & function, std::index_sequence<Is...>)
        {
            function(std::get<Is>(m_parameters)...);
        }

        ThreadPool<Args...> & m_parent;
        std::atomic_bool m_running;
        std::atomic_bool m_stopped;
        std::thread m_thread;
        std::tuple<Args...> m_parameters;
        std::mutex m_mutex;
        std::queue<TaskController<>> m_stopQueue;
        Semaphore m_workSemaphore;

    };

    /**
    * Thead pool class.
    *
    */
    template<typename ... Args>
    class ThreadPool
    {

    public:

        ThreadPool() :
            m_running(false),
            m_size(0),
            m_max(0)
        { }

        ~ThreadPool()
        {
            stop();

            for (auto it = m_threadSet.begin(); it != m_threadSet.end(); it++)
            {
                delete *it;
            }
        }

        bool start(const size_t size, const size_t max, std::function<void(Args...)> function)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            // Check if it's possible to start.
            if (m_running)
            {
                return false;
            }
            m_running = true;

            // Initialize.
            m_function = function;
            m_size = size;
            m_max = max > size ? max : size;

            allocateAppendJobs(m_size);

            return true;
        }

        void stop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (!m_running)
            {
                return;
            }

            std::vector<Task<>> stopTasks;
            for (auto it = m_threadSet.begin(); it != m_threadSet.end(); it++)
            {
                stopTasks.push_back((*it)->stop());
            }

            for (auto it = stopTasks.begin(); it != stopTasks.end(); it++)
            {
                it->wait();
                if (!it->successful())
                {
                    throw std::runtime_error("Failed to stop thread pool.");
                }
            }

            m_running = false;
        }

        bool work(Args ... args)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            // Allocate more jobs if needed.
            if (!m_threadQueue.size())
            {
                const size_t currentAllocCount = m_threadSet.size();
                if (currentAllocCount == m_max)
                {
                    return false;
                }

                size_t allocateCount = std::min<size_t>(m_threadSet.size() - m_size, 2);

                if (currentAllocCount + allocateCount >= m_max)
                {
                    allocateCount = m_max - currentAllocCount;
                }

                allocateAppendJobs(allocateCount);
            }

            auto * threadJob = m_threadQueue.front();
            threadJob->work(args...);
            m_threadQueue.pop();

            return true;
        }

        void addToQueue(ThreadJob<Args...> * threadJob)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_threadSet.find(threadJob) == m_threadSet.end())
            {
                throw std::runtime_error("Failed to put back job to queue.");
                return;
            }

            m_threadQueue.push(threadJob);
        }
        
    private:

        void allocateAppendJobs(const size_t count)
        {
            for (size_t i = 0; i < count; i++)
            {
                ThreadJob<Args...> * newJob = new ThreadJob<Args...>(*this, m_function);
                m_threadQueue.push(newJob);
                m_threadSet.insert(newJob);
            }
        }

        std::atomic_bool m_running;
        size_t m_size;
        size_t m_max;
        std::function<void(Args...)> m_function;
        std::queue<ThreadJob<Args...> *> m_threadQueue;
        std::set<ThreadJob<Args...> *> m_threadSet;
        std::mutex m_mutex;

    };

}

//#include "wepp/threadPool.inl"

#endif