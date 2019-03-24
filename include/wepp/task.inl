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

    // Task.
    template<typename Return>
    Task<Return>::Task() :
        m_members(std::make_shared<Members>())
    { }

    template<typename Return>
    template<typename ... Args>
    Task<Return>::Task(Args ... args) :
        m_members(std::make_shared<Members>(args...))
    { }


    template<typename Return>
    Task<Return>::Task(const Task<Return> & task) :
        m_members(task.m_members)
    { }

    template<typename Return>
    Task<Return>::Task(const TaskController<Return> & taskController) :
        m_members(taskController.m_members)
    { }

    template<typename Return>
    Task<Return>::~Task()
    { }

    template<typename Return>
    Return & Task<Return>::operator()()
    {
        return m_members->m_value;
    }

    template<typename Return>
    const Return & Task<Return>::operator()() const
    {
        return m_members->m_value;
    }

    template<typename Return>
    const TaskStatus Task<Return>::status() const
    {
        std::unique_lock<std::mutex> lock(m_members->m_mutex);
       return m_members->m_status;
    }

    template<typename Return>
    bool Task<Return>::pending() const
    {
        std::unique_lock<std::mutex> lock(m_members->m_mutex);
        return m_members->m_status == TaskStatus::Pending;
    }

    template<typename Return>
    bool Task<Return>::successful() const
    {
        std::unique_lock<std::mutex> lock(m_members->m_mutex);
        return m_members->m_status == TaskStatus::Successful;
    }

    template<typename Return>
    bool Task<Return>::failed() const
    {
        std::unique_lock<std::mutex> lock(m_members->m_mutex);
        return m_members->m_status == TaskStatus::Failed;
    }

    template<typename Return>
    const bool Task<Return>::timeout() const
    {
        std::unique_lock<std::mutex> lock(m_members->m_mutex);
        return m_members->m_timeout;
    }

    template<typename Return>
    Task<Return> & Task<Return>::wait()
    {
        std::unique_lock<std::mutex> lock(m_members->m_mutex);

        while (m_members->m_status == TaskStatus::Pending)
        {
            m_members->m_condition.wait(lock);
        }

        return *this;
    }

    template<typename Return>
    Task<Return> & Task<Return>::wait(const std::chrono::duration<double> timeout)
    {
        std::unique_lock<std::mutex> lock(m_members->m_mutex);

        m_members->m_timeout = false;
        const auto now = std::chrono::system_clock::now();

        while (m_members->m_status == TaskStatus::Pending)
        {
            if (m_members->m_condition.wait_until(lock, now + timeout) == std::cv_status::timeout)
            {
                m_members->m_timeout = true;
                break;
            }
        }

        return *this;
    }

    template<typename Return>
    bool Task<Return>::operator ==(const TaskStatus & status) const
    {
        return m_members->m_status == status;
    }

    template<typename Return>
    bool Task<Return>::operator !=(const TaskStatus & status) const
    {
        return m_members->m_status != status;
    }



    // Task members.
    template<typename Return>
    Task<Return>::Members::Members() :
        m_status(TaskStatus::Pending),
        m_timeout(false)
    {}

    template<typename Return>
    template<typename ... Args>
    Task<Return>::Members::Members(Args ... args) :
        m_status(TaskStatus::Pending),
        m_value(args...),
        m_timeout(false)
    {}



    // Task Controller
    template<typename Return>
    TaskController<Return>::TaskController() :
        Task<Return>()
    { }

    template<typename Return>
    template<typename ... Args>
    TaskController<Return>::TaskController(Args ... args) :
        Task<Return>(args...)
    { }

    template<typename Return>
    TaskController<Return>::~TaskController()
    { }

    template<typename Return>
    TaskController<Return> & TaskController<Return>::fail()
    {
        std::unique_lock<std::mutex> lock(this->m_members->m_mutex);
        this->m_members->m_status = TaskStatus::Failed;
        this->m_members->m_condition.notify_all();
        return *this;
    }

    template<typename Return>
    TaskController<Return> & TaskController<Return>::finish()
    {
        std::unique_lock<std::mutex> lock(this->m_members->m_mutex);
        this->m_members->m_status = TaskStatus::Successful;
        this->m_members->m_condition.notify_all();
        return *this;
    }

}