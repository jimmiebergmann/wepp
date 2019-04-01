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
#include <iostream>

namespace Wepp
{

    namespace Http
    {

        Server::Server() :
            m_state(State::Stopped)
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
                    std::lock_guard<std::mutex> lock(m_mutex);

                    if (m_state != State::Starting)
                    {
                        failedStarting = true;
                        break;
                    }

                    const size_t poolMin = 1;
                    const size_t poolMax = 100;

                    if(!m_receivePool.start(poolMin, poolMax,
                        [this](Priv::HttpReceiver & receiver, std::shared_ptr<Socket::TcpSocket> socket)
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

                        }
                    ).wait().successful())
                    {
                        failedStarting = true;
                        break;
                    }

                    if (!m_listener.start(port, endpoint).wait().successful())
                    {
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


                // Listen loop.
                while (m_state == State::Started)
                {
                    auto connection = m_listener.listen().wait();

                    if (m_state != State::Started)
                    {
                        break;
                    }

                    if (connection.successful())
                    {
                       m_receivePool.enqueue(connection());
                    }
                    else
                    {
                        std::cerr << "Call to listen failed.";
                    }
                }

                // Finishing.
                handleStop();
            });

            return m_startTask;
        }

        Task<> Server::stop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if(m_state == State::Stopping)
            {
                return m_stopTask;
            }
            else if (m_state == State::Stopped)
            {
                return m_stopTask.finish();
            }

            m_state = State::Stopping;
            m_listener.stop();

            return m_stopTask;
        }

        Task<> Server::stopped()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_stopTask;
        }

        void Server::handleStop()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_receivePool.stop().wait();
            m_listener.stop().wait();

            m_state = State::Stopped;
            m_stopTask.finish();
        }

    }

}
