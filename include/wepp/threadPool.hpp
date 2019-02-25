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


    /**
    * Thread pool worker.
    *
    */
    template<typename ... Args>
    class ThreadWorker
    {

    public:

        /**
        * Constructor.
        *
        */
        ThreadWorker(ThreadPool<Args...> & parent);

        /**
        * Destructor.
        *
        */
        ~ThreadWorker();

        /**
        * Start the worker thread.
        *
        */
        bool start(std::function<void(Args...)> function);

        /**
        * Stop the worker thread.
        *
        */
        Task<> stop();

        /**
        * Execute job.
        *
        */
        void execute(std::tuple<Args...> args);

    private:

        /**
        * Internal helper function, making it possible to pass a tuple as parameter pack.
        *
        */
        void callFunction();

        template<std::size_t... Is>
        void callFunctionHelper(std::index_sequence<Is...>);

        ThreadPool<Args...> & m_parent;             /**< Reference of parent, thread pool. */
        std::atomic_bool m_running;                 /**< Flag, indicating if the worker is running. */
        std::thread m_thread;                       /**< Work thread. */
        std::function<void(Args...)> m_function;    /**< Execution function. */
        std::tuple<Args...> m_parameters;           /**< Latest parameters to execute, from the thread pool. */
        std::mutex m_mutex;                         /**< Mutex protecting. */
        Semaphore m_executeSemaphore;               /**< Sempahore triggering executing. */
        TaskController<> m_stopTask;                /**< Stopping task. */

    };

    /**
    * Single function thread pool.
    * Makes it possible to execute a single function on multiple threads, but with different arguments.
    *
    */
    template<typename ... Args>
    class ThreadPool
    {

    public:

        /**
        * Default constructor.
        *
        */
        ThreadPool();

        /**
        * Destructor.
        *
        */
        ~ThreadPool();

        /**
        * Deletet copy constructor.
        *
        */
        ThreadPool(const ThreadPool &) = delete;

        /**
        * Start the thead pool.
        *
        * @param[in] function - Function to execute by workers.
        * @param[in] minWorkers - Initial numbers of allocated workers.
        * @param[in] maxWorkers - Maximum numbers of allocated workers. Set to same value as minWorkers if lower than that value.
        *
        */
        Task<> start(std::function<void(Args...)> function, const size_t minWorkers, const size_t maxWorkers = 0);

        /**
        * Stop the thead pool.
        *
        */
        Task<> stop();

        /**
        * Enqueue jobs for workers.
        *
        * @return true if successfully enqueued job, else false.
        *
        */
        bool enqueue(Args ... args);

    private:

        /**
        * Allocates more workers and places them in queue to receive jobs.
        *
        * @param[in] count - Number of workers to allocate.
        *
        */
        void allocateWorkers(const size_t count);

        /**
        * Enqueue worker.
        *
        */
        void enqueueWorker(ThreadWorker<Args...> * worker);

        std::atomic_bool m_running;                         /**< Flag indicating if the pool is running. */
        TaskController<> m_stopTask;                        /**< Stopping task. */
        size_t m_minWorkers;                                /**< Minimum numbers of workers beging allocated. */   
        size_t m_maxWorkers;                                /**< Maximum numbers of workers beging allocated. */    
        std::queue<ThreadWorker<Args...> *> m_workerQueue;  /**< Queue of workers available to receive work. */
        std::set<ThreadWorker<Args...> *> m_workerSet;      /**< Set of all workers in this pool. */
        std::queue<std::tuple<Args...>> m_jobQueue;         /**< Queue of jobs not yet distributed. */
        std::function<void(Args...)> m_function;            /**< Function being executed by workers. */
        std::thread m_thread;                               /**< Thread distrubuting jobs and [de]/allocating workers. */
        std::mutex m_mutex;                                 /**< Mutex protecting the queue/set containers. */
        Semaphore m_workerSemaphore;                        /**< Sempahore triggering available workers. */
        Semaphore m_jobSemaphore;                           /**< Sempahore triggering available jobs. */

    };

}

#include "wepp/threadPool.inl"

#endif