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

#include "wepp/http/server.hpp"
#include <chrono>
#include <iostream>

namespace Wepp
{

    namespace Http
    {

        Server::Server() :
            m_running(false),
            m_stopped(true)
        { }

        Server::~Server()
        {
            stop().wait();
            if (m_thread.joinable())
            {
                m_thread.join();
            }
        }

        Task<> Server::start()
        {
            std::lock_guard<std::mutex> lock(m_stopQueueMutex);

            TaskController<> task(false);

            if (m_running)
            {
                return task.finish();
            }
            if (!m_stopped)
            {
                return task.fail();
            }
            m_stopped = false;

            m_stopTask = TaskController<>();
            
            // Start main thread.
            m_thread = std::thread([this, task]() mutable
            {
                // Start recieve pool.
                m_recivePool.start([this](std::shared_ptr<Socket::TcpSocket> socket)
                {
                    //std::cout << "Receiving data from peer..." << std::endl;

                    const size_t bufferSize = 1024;
                    char buffer[bufferSize + 1];

                    int size = socket->receive(buffer, bufferSize);
                    if (size < 0)
                    {
                        std::cout << "Native error: " << Socket::TcpSocket::getLastError() << std::endl;
                    }
                    else if (size >= 0)
                    {
                        buffer[size] = 0;
                        static size_t counter = 0;
                        counter++;
                        std::cout << "Count: " << counter << std::endl;
                        std::cout << buffer << std::endl;
                    }

                }, 10, 100).wait();

                // Start listener.
                if (!m_listener.start("127.0.0.1", 80).wait().successful())
                {
                    m_stopped = true;
                    task.fail();
                    return;
                }
  
                // Set task as finished.
                m_running = true;
                m_stopped = false;
                task.finish();

                // Listen loop.
                while (m_running)
                {
                    auto connection = m_listener.listen().wait();

                    if (!m_running)
                    {
                        break;
                    }

                    if (connection.successful())
                    {
                        if (!m_recivePool.enqueue(connection()))
                        {
                            throw std::runtime_error("Failed to start work.");
                        }
                    }
                    else
                    {
                        std::cerr << "Failed listen task.";
                    }
                }

                handleStop();               
            });
           
            return task;
        }

        Task<> Server::stop()
        {
            std::lock_guard<std::mutex> lock(m_stopQueueMutex);

            if (m_stopped)
            {
                return m_stopTask.finish();
            }

            m_listener.stop();

            m_running = false;
            return m_stopTask;
        }

        void Server::handleStop()
        {
            std::lock_guard<std::mutex> lock(m_stopQueueMutex);

            m_recivePool.stop().wait();
            m_listener.stop().wait();
            
            m_stopped = true;
            m_stopTask.finish();
        }

    }

}