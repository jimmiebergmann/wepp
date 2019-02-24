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

#ifndef WEPP_TASK_HPP
#define WEPP_TASK_HPP

#include "wepp/build.hpp"
#include <memory>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>

/**
* Wepp namespace.
*
*/
namespace Wepp
{

    template<typename Return> class TaskController;

    /**
    * Enumerator of status codes of task.
    *
    */
    enum class TaskStatus
    {
        Pending,        /**< The task is not yet finished. */
        Successful,     /**< The task is finished and succeded. */
        Failed,         /**< The task failed. */
    };


    /**
    * Task class.
    *
    *
    */
    template<typename Return = bool>
    class Task
    {

    public:

        /**
        * Constuctor.
        *
        */
        Task();

        /**
        * Constuctor.
        * 
        * @param[in] args - List of arguments of initialization values for the return value.
        *
        */
        template<typename ... Args>
        Task(Args ... args);

        /**
        * Casting task controller to task.
        *
        */
        Task(const TaskController<Return> & taskController);

        /**
        * Destructor.
        *
        */
        virtual ~Task();

        /**
        * Value access operator.
        *
        */
        Return & operator()();

        /**
        * Constant value access operator.
        *
        */
        const Return & operator()() const;

        /**
        * Gets current status.
        *
        */
        const TaskStatus status() const;

        /**
        * Check if task is pending.
        *
        */
        bool pending() const;

        /**
        * Check if task is successful.
        *
        */
        bool successful() const;

        /**
        * Check if task failed.
        *
        */
        bool failed() const;

        /**
        * Check if task timed out during the last wait(...) call.
        *
        */
        const bool timeout() const;

        /**
        * Wait until the task succeed or fail.
        * Function terminates instantly if current status is not equal to Status::Pending.
        *
        * @remark Make sure to check status() to evalutate the task result after wait() is finished.
        *
        * @return reference to this task.
        *
        * @see status()
        *
        */
        Task<Return> & wait();

        /**
        * Wait until the task is complete.
        * Function terminates instantly if current status is not equal to Status::Pending.
        *
        * @param[in] timeout - Maximum duration of wait.
        *
        * @remark Make sure to check status() to evalutate the task result after wait() is finished.
        *         The timeout flag is set to true if the timeout is reached before finishing the task.
        *
        * @return reference to this task.
        *
        * @see status()
        * @see timeout()
        *
        */
        Task<Return> & wait(const std::chrono::duration<double> timeout);

        /**
        * Equal to Status comparison.
        *
        */
        bool operator ==(const TaskStatus & status) const;

        /**
        * Not equal to Status comparison.
        *
        */
        bool operator !=(const TaskStatus & status) const;

    protected:

        /**
        * Members of task class.
        * This struct is used by the Task class, wrapped by shared_ptr to make sure copies of the task share the same data.
        *
        */
        struct Members
        {

            /**
            * Constructor.
            *
            */
            Members();

            /**
            * Constructor, with parameters for return value.
            *
            */
            template<typename ... Args>
            Members(Args ... args);

            std::atomic<TaskStatus> m_status;                        /**< Status of task. */
            Return m_value;                         /**< Return value of task. */
            std::atomic_bool m_timeout;                         /**< Flag indicating if wait() timed out.*/
            std::mutex m_mutex;
            std::condition_variable m_condition;
        };

        std::shared_ptr<Members> m_members; /**< Shared data members of task. */

    };

    /**
    * Task controller class.
    *
    *
    */
    template<typename Return = bool>
    class TaskController : public Task<Return>
    {

    public:

        /**
        * Constuctor.
        *
        */
        TaskController();

        /**
        * Constuctor.
        *
        * @param[in] args - List of arguments of initialization values.
        *
        */
        template<typename ... Args>
        TaskController(Args ... args);

        /**
        * Destructor.
        *
        */
        ~TaskController();

        /**
        * Mark the task as failed.
        * All wait() operators are terminated.
        *
        * @return Reference to task.
        *
        */
        TaskController & fail();

        /**
        * Mark the task as successful.
        * All wait() operators are terminated.
        *
        * @return Reference to task.
        *
        */
        TaskController & finish();

    };

}

#include "wepp/task.inl"

#endif