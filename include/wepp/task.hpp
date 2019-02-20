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

/**
* Wepp namespace.
*
*/
namespace Wepp
{

    template<typename Return> class TaskController;

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
        * Enumerator of status codes of task.
        *
        */
        enum class Status
        {
            Pending,        /**< The task is not yet finished. */
            Successful,     /**< The task is finished and succeded. */
            Failed,         /**< The task failed. */
        };

        /**
        * Constuctor.
        *
        */
        Task() :
            m_members(std::make_shared<Members>())
        { }

        /**
        * Constuctor.
        * 
        * @param[in] args - List of arguments of initialization values for the return value.
        *
        */
        template<typename ... Args>
        Task(Args ... args) :
            m_members(std::make_shared<Members>(args...))
        { }


        /**
        * Casting task controller to task.
        *
        */
        Task(const TaskController<Return> & taskController) :
            m_members(taskController.m_members)
        { }

        /**
        * Destructor.
        *
        */
        virtual ~Task()
        { }

        /**
        * Value access operator.
        *
        */
        Return & operator()()
        {
            return m_members->m_value;
        }

        /**
        * Constant value access operator.
        *
        */
        const Return & operator()() const
        {
            return m_members->m_value;
        }

        /**
        * Gets current status.
        *
        */
        const Status status() const
        {
            return m_members->m_status;
        }

        /**
        * Check if the current status is set to Status::Successful.
        *
        */
        bool successful() const
        {
            return m_members->m_status == Status::successful;
        }

        /**
        * Check if the current status is set to Status::Failed.
        *
        */
        bool failed() const
        {
            return m_members->m_status == Status::Failed;
        }

        /**
        * Check if task timed out during the last wait(...) call.
        *
        */
        const Status timeout() const
        {
            return m_members->m_timeout;
        }

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
        Task<Return> & wait()
        {
            
            return *this;
        }

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
        Task<Return> & wait(const std::chrono::duration<double> timeout)
        {
            /*using namespace std::chrono_literals;
            auto day = 24h;
            auto halfhour = 0.0s;*/

            return *this;
        }

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
            Members() :
                m_status(Status::Pending),
                m_timeout(false)
            {}

            /**
            * Constructor, with parameters for return value.
            *
            */
            template<typename ... Args>
            Members(Args ... args) :
                m_status(Status::Pending),
                m_value(args...),
                m_timeout(false)
            {}

            Status m_status; /**< Status of task. */
            Return m_value;  /**< Return value of task. */
            bool m_timeout;  /**< Flag indicating if wait() timed out.*/
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
        TaskController() :
            Task<Return>()
        { }

        /**
        * Constuctor.
        *
        * @param[in] args - List of arguments of initialization values.
        *
        */
        template<typename ... Args>
        TaskController(Args ... args) :
            Task<Return>(args...)
        { }

        /**
        * Destructor.
        *
        */
        ~TaskController()
        { }

        /**
        * Mark the task as failed.
        * All wait() operators are terminated.
        *
        */
        void fail()
        {
            this->m_members->m_status = Task<Return>::Status::Failed;
        }

        /**
        * Mark the task as successful.
        * All wait() operators are terminated.
        *
        */
        void finish()
        {
            this->m_members->m_status = Task<Return>::Status::Successful;
        }

    };

}

#include "wepp/task.inl"

#endif