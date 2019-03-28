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
#include "wepp/priv/httpReceiver.hpp"
#include <chrono>
#include <iostream>

#define WEPP_COMBINE_HELPER(X,Y) X##Y
#define WEPP_COMBINE(X, Y) WEPP_COMBINE_HELPER(X,Y)
#define WEPP_DO_ONCE for(size_t WEPP_COMBINE(wepp_do_once_var_, __LINE__) = 0; WEPP_COMBINE(wepp_do_once_var_, __LINE__) < 1; WEPP_COMBINE(wepp_do_once_var_, __LINE__)++)

namespace Wepp
{

    namespace Http
    {

        Server::Server() :
            m_starting(false),
            m_started(false),
            m_stopped(true),
            m_stopping(false)
        { }

        Server::~Server()
        {           
            stop().wait();
            if (m_thread.joinable())
            {
                m_thread.join();
            }
        }

        Task<> Server::start(const uint16_t port, const std::string & endpoint)
        {
            std::lock_guard<std::mutex> lock(m_mainMutex);

            if (!m_stopped || m_started || m_starting || m_stopping)
            {
                return TaskController<>().fail();
            }
            
            m_starting = true;
            m_stopTask = TaskController<>();
            m_startTask = TaskController<>();

            // Start main thread.
            m_thread = std::thread([this, port, endpoint]() mutable
            {
                WEPP_DO_ONCE
                {
                    std::lock_guard<std::mutex> threadLock(m_threadMutex);

                    if (m_stopping)
                    {
                        break;
                    }

                    const size_t poolMin = 10;
                    const size_t poolMax = 100;

                    m_receivePool.start(poolMin, poolMax, [this](Priv::HttpReceiver & receiver, std::shared_ptr<Socket::TcpSocket> socket)
                    {
                        Router::CallbackFunc callbackFunction = nullptr;
                        Request request;
                        Response response;

                        const auto status = receiver.receive(socket, request, response,

                            // On request.
                            [this, &callbackFunction](Request & request, Response & response) mutable -> bool
                            {
                                std::vector<std::string> matches;
                                callbackFunction = route.find(request.method(), request.resource(), matches);
                                if (callbackFunction == nullptr)
                                {
                                    response.status(Status::NotFound);
                                    return false;
                                }
                                return true;
                            }
                        );

                        // Execute callback.
                        if (status == Priv::HttpReceiver::Status::Ok && callbackFunction)
                        {
                            callbackFunction(request, response);
                        }

                        if (status != Priv::HttpReceiver::Status::Disconnected)
                        {
                            // Send response
                            auto & body = response.body();

                            std::string responseStart =
                                "HTTP/1.1 ";
                            responseStart += std::to_string(static_cast<unsigned long long>(response.status()));
                            responseStart += " " + getStatusAsString(response.status());
                            responseStart += "\r\n";
                            responseStart +=
                                "Server: Wepp\r\n"
                                "Content-Length: " + std::to_string(body.size()) + "\r\n"
                                "Connection: Closed\r\n\r\n";

                            socket->send(responseStart);

                            const size_t bodySize = body.size();
                            if (bodySize)
                            {
                                socket->send(body.data(), bodySize);
                            }
                            
                        }

                    }).wait();


                    // Start listener.
                    if (!m_listener.start(port, endpoint).wait().successful())
                    {
                        break;
                    }

                    // Set task as finished.
                    m_starting = false;
                    m_started = true;
                    m_stopped = false;
                    m_startTask.finish();
                }

                // Listen loop.
                while (m_started && !m_stopping)
                {
                    auto connection = m_listener.listen().wait();

                    if (m_stopping)
                    {
                        break;
                    }

                    if (connection.successful())
                    {
                       m_receivePool.enqueue(connection());
                    }
                    else
                    {
                        std::cerr << "Failed listen task.";
                    }
                }

                handleStop();
            });

            return m_startTask;
        }

        Task<> Server::stop()
        {
            std::lock_guard<std::mutex> lock(m_mainMutex);
            std::lock_guard<std::mutex> threadLock(m_threadMutex);

            if (m_stopped)
            {
                return m_stopTask.finish();
            }
            m_stopping = true;

            m_listener.stop();

            return m_stopTask;
        }

        void Server::handleStop()
        {
            std::lock_guard<std::mutex> lock(m_mainMutex);

            m_receivePool.stop().wait();
            m_listener.stop().wait();

            if (!m_started)
            {
                m_startTask.fail();
            }

            m_stopping = false;
            m_stopped = true;
            m_started = false;
            m_stopTask.finish();
        }

    }

}
