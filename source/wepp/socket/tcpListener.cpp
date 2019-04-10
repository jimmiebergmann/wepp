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

#include "wepp/socket/tcpListener.hpp"
#include "wepp/socket/platform/socketFunctions.hpp"
#include <iostream>

namespace Wepp
{

    namespace Socket
    {

        TcpListener::TcpListener() :
            m_state(State::Stopped)
        { }

        TcpListener::~TcpListener()
        {
            stop().wait();
            if (m_thread.joinable())
            {
                m_thread.join();
            }
        }

        Task<> TcpListener::start(const uint16_t port, const std::string endpoint)
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

            // Start main thread.
            m_thread = std::thread([this, port, endpoint]() mutable
            {
                // Initialize listener.
                WEPP_DO_ONCE
                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    if (m_state != State::Starting)
                    {
                        m_startTask.fail();
                        break;
                    }

                    if (m_listenSocket.listen(port, endpoint) != Socket::Status::Successful)
                    {
                        m_startTask.fail();
                        break;
                    }

                    m_state = State::Started;
                    m_startTask.finish();
                }

                // Main loop.
                while (m_state == State::Started)
                {
                    m_sempahore.wait();
                    
                    if (m_state != State::Started)
                    {
                        break;
                    }

                    // Store copy of listener socket.
                    Socket::Handle listenHandle;
                    TaskController<std::shared_ptr<TcpSocket>> listenTask;
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);

                        if (m_state != State::Started)
                        {
                            break;
                        }

                        listenHandle = m_listenSocket.getHandle();

                        listenTask = m_listenQueue.front();
                        m_listenQueue.pop();
                    }
                    
                    // Accept connection.
                    TcpSocket connection;
                    TcpSocket tempListenSocket(listenHandle);
                    auto status = tempListenSocket.accept(connection);
                    tempListenSocket.releaseHandle(); // Make sure to release handle of temporary socket, or it is closed at variable destruction.

                    // Check if running, and error check accepted socket.
                    if (m_state != State::Started ||status != Socket::Status::Successful)
                    {
                        //std::cerr << "Accept failed with error: " << Socket::getLastError() << std::endl;
                        listenTask.fail();
                        continue;
                    }

                    listenTask() = std::make_shared<TcpSocket>(connection);
                    listenTask.finish();
                }

                handleStop();

            });

            return m_startTask;
        }

        Task<> TcpListener::stop()
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
            m_listenSocket.close();
            m_sempahore.notifyOne();

            return m_stopTask;
        }

        Task<std::shared_ptr<TcpSocket>> TcpListener::listen()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            TaskController<std::shared_ptr<TcpSocket>> task;
            if (m_state != State::Started)
            {
                return task.fail();
            }

            m_listenQueue.push(task);
            m_sempahore.notifyOne();
            return task;
        }

        void TcpListener::handleStop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_listenSocket.close();

            while (m_listenQueue.size())
            {
                m_listenQueue.front().fail();
                m_listenQueue.pop();
            }

            m_state = State::Stopped;
            m_stopTask.finish();        
        }

    }

}
