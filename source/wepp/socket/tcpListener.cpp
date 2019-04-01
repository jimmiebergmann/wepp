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
                bool failedStarting = false;

                WEPP_DO_ONCE
                {
                    if (m_state != State::Starting)
                    {
                        failedStarting = true;
                        break;
                    }

                    // Get address as integer.
                    uint32_t endpointAddr = htonl(INADDR_ANY);
                    if (endpoint.size() && inet_pton(AF_INET, endpoint.c_str(), &endpointAddr) != 1)
                    {
                        //std::cerr << "Invalid ip address: " << address << " - error: " << Socket::getLastError() << std::endl;
                        failedStarting = true;
                        break;
                    }

                    // Create listen socket.
                    m_handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                    if (WeppIsSocketInvalid(m_handle))
                    {
                        //std::cerr << "Socket creation failed with error: " << Socket::getLastError() << std::endl;
                        m_handle = 0;
                        failedStarting = true;
                        break;
                    }
                    // Bind listen socket.
                    sockaddr_in service;
                    service.sin_family = AF_INET;
                    service.sin_addr.s_addr = endpointAddr;
                    service.sin_port = htons(port);

                    int ret = 0;
                    if ((ret = ::bind(m_handle, reinterpret_cast<const WEPP_SOCKADDR_TYPE *>(&service), sizeof(service))) != 0)
                    {
                        //std::cerr << "Bind function failed with error: " << Socket::getLastError() << std::endl;
                        
                        /*stopListen();
                        task.fail();
                        return;*/
                        failedStarting = true;
                        break;
                    }

                    if (::listen(m_handle, SOMAXCONN) != 0)
                    {
                        //std::cerr << "Listen function failed with error: " << Socket::getLastError() << std::endl;
                        failedStarting = true;
                        break;
                    }
                }

                // Set task as finished or failed.
                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    if (!failedStarting)
                    {
                        m_startTask.finish();

                        if (m_state == State::Starting)
                        {
                            m_state = State::Started;
                        }
                    }
                    else
                    {
                        m_startTask.fail();
                    }
                }

                while (m_state == State::Started)
                {
                    m_sempahore.wait();
                    
                    if (m_state != State::Started)
                    {
                        break;
                    }

                    // Store handler.
                    Handle listenHandler;
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);

                        if (m_state != State::Started)
                        {
                            break;
                        }

                        listenHandler = m_handle;
                    }

                    // Accept the connection.
                    Socket::Handle connection = ::accept(listenHandler, NULL, NULL);

                    // Check if listener still is running.
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);

                        if (m_state != State::Started)
                        {
                            break;
                        }

                        if (WeppIsSocketInvalid(connection))
                        {
                            //std::cerr << "accept failed with error: " << Socket::getLastError() << std::endl;
                            continue;
                        }
                    }

                    // Set native handle of task.
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);

                        TaskController<std::shared_ptr<TcpSocket>> connTask = m_listenQueue.front();
                        m_listenQueue.pop();
                        connTask() = std::make_shared<TcpSocket>(connection);
                        connTask.finish();
                    }
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
            
            if (m_handle)
            {
                WeppCloseSocket(m_handle);
                m_handle = 0;
            }

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

            if (m_handle)
            {
                WeppCloseSocket(m_handle);
                m_handle = 0;
            }

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
