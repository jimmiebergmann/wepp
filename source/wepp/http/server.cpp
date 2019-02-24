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

        static void foo(Task<bool> task)
        {
            int wat = 5;
        }

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
            
            // Start main thread.
            m_thread = std::thread([this, task]() mutable
            {
                // Start recieve pool.
                m_recivePool.start(10, 100, [](std::shared_ptr<Socket::TcpSocket> socket)
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
                        std::cout << buffer << std::endl;
                    }

                });

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
                    auto connection = m_listener.listen().wait(std::chrono::seconds(1));

                    if (!m_running)
                    {
                        break;
                    }

                    if (connection.successful())
                    {
                        if (!m_recivePool.work(connection()))
                        {
                            throw std::runtime_error("Failed to start work.");
                        }
                    }
                }

                handleStop();
                
            });
           

            return task;
        }

        Task<> Server::stop()
        {
            std::lock_guard<std::mutex> lock(m_stopQueueMutex);

            TaskController<> task;

            if (m_stopped)
            {
                return task.finish();
            }

            m_stopQueue.push(task);

            m_running = false;
            return task;
        }

        void Server::handleStop()
        {
            std::lock_guard<std::mutex> lock(m_stopQueueMutex);

            while (m_stopQueue.size())
            {
                m_stopQueue.front().finish();
                m_stopQueue.pop();
            }

            m_recivePool.stop();

            m_stopped = true;
        }

    }

}