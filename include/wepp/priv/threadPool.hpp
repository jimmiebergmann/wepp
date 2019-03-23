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
#include "wepp/semaphore.hpp"
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <set>
#include <queue>
#include <tuple>
#include <utility>


/**
* Wepp namespace.
*
*/
namespace Wepp
{

    /**
    * Private namespace.
    *
    */
    namespace Priv
    {

        template<typename ... Args>
        class ThreadPoolBase;

        /**
        * Thread pool worker, used for inheritance to implement custom thread pool logics.
        *
        * @tparam Args  - Arguments of execute function.
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
            ThreadWorker(ThreadPoolBase<Args...> * pool);

            /**
            * Destructor.
            *
            */
            virtual ~ThreadWorker();

            /**
            * Notify about new work.
            *
            */
            void work(Args ... args);

        private:

            /**
            * Virtual function, called when enqueued workers are executed.
            *
            * @tparam Args  - Arguments of execute function.
            *
            */
            virtual void execute(Args ... args) = 0;

            void executeVirtual();

            /**
            * Helper function for executing function. Casting tuple as parameter pack for worker execution function.
            *
            */
            template<std::size_t... Is>
            void executeVirtualHelper( std::index_sequence<Is...>);

            std::thread                 m_thread;
            std::mutex                  m_mutex;
            std::atomic_bool            m_running;
            std::tuple<Args...>         m_args;
            Semaphore                   m_semaphore;
            ThreadPoolBase<Args...> *   m_pool;

        };


        /**
        * base class of thread pool worker.
        *
        */
        template<typename ... Args>
        class ThreadPoolBase
        {

        public:

            /**
            * Returning worker to thread pool.
            *
            * @throw std::runtime_error if worker isn't part of this pool.
            *
            */
            virtual void returnWorker(ThreadWorker<Args...> * worker) = 0;

        };


        /**
        * Thread pool, allocating and managing workers of given template type.
        * Any allocated resource is owned and handled by this class.
        *
        * @tparam T     - Type of worker, must inherit from ThreadWorker.
        * @tparam Args  - Arguments of enqueue function, passed worker's execute function.
        *
        */
        template<typename T, typename ... Args>
        class ThreadPool : public ThreadPoolBase<Args...>
        {

        public:

            typedef ThreadWorker<Args...> Worker;

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
            * Start the thead pool.
            *
            * @param[in] minWorkers - Initial numbers of allocated workers.
            * @param[in] maxWorkers - Maximum numbers of allocated workers. Set to same value as minWorkers if lower than that value.
            * @param[in] cargs      - Constructor parameters of worker.
            *
            * @tparam CArgs - Constructor parameters of worker.
            *
            */
            template<typename ... CArgs>
            Task<> start(const size_t minWorkers, const size_t maxWorkers, CArgs ... cargs);

            /**
            * Stop the thead pool.
            *
            */
            Task<> stop();

            /**
            * Enqueue work and pass it to worker when any worker is available.
            *
            */
            void enqueue(Args ... args);

            /**
            * Returning worker to thread pool.
            *
            * @throw std::runtime_error if worker isn't part of this pool.
            *
            */
            virtual void returnWorker(Worker * worker);

        private:

            typedef std::tuple<Args...> Work;           /**< Typedef of work.*/

            /**
            * Function for cleaning up after startup.
            *
            */
            void cleanup();

            /**
            * Function for executing worker function. Internally calling executeWorkerHelper to caset tuple as parameter pack.
            *
            */
            void executeWorker(Worker * worker, Work & work);

            /**
            * Helper function for executing worker function. Casting tuple as parameter pack for worker execution function.
            *
            */
            template<std::size_t... Is>
            void executeWorkerHelper(Worker * worker, Work & work, std::index_sequence<Is...>);

            std::atomic_bool        m_started;              /**< Flag indicating if the pool is started and running.*/
            std::atomic_bool        m_stopped;              /**< Flag indicating if the pool is completely stopped.*/
            TaskController<>        m_startedTask;          /**< Task controller for indicating completion of pool start*/
            TaskController<>        m_stoppedTask;          /**< Task controller for indicating completion of pool stoppage.*/
            std::thread             m_thread;               /**< Thread distrubuting work and [de]/allocating workers. */
            std::mutex              m_mutex;                /**< Mutex protecting the queue/set containers. */

            std::set<Worker *>      m_workerSet;            /**< Set of allocated workers.*/
            std::queue<Worker *>    m_workerQueue;          /**< Queue of available workers.*/
            std::queue<Work>        m_workQueue;            /**< Queue of enqueued worke.*/
            Semaphore               m_availableSemaphore;   /**< Semaphore notifying about available workers.*/
            Semaphore               m_enqueueSemaphore;     /**< Semaphore notifying about enqueued work.*/

            size_t                  m_minWorkers;           /**< Minimum numbers of allocated workers.*/
            size_t                  m_maxWorkers;           /**< Maximum numbers of possible allocated workers in this pool.*/

        };

    }

}

#include "wepp/priv/threadPool.inl"

#endif
