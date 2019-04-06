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
            m_state(State::Stopped),
            m_defaultOnError([](Response & response)
            {
                response << std::to_string(static_cast<uint32_t>(response.status())) << " - " << getStatusAsString(response.status());
            })
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
                            Router::CallbackFunc routeFunction = nullptr;
                            Request request;
                            Response response;
                            Response errorResponse;

                            auto status = receiver.receive(socket, request, response,

                                // On request.
                                [this, &routeFunction](Request & request, Response & response) mutable -> bool
                                {
                                    std::vector<std::string> matches;
                                    routeFunction = route.find(request.method(), request.resource(), matches);
                                    if (routeFunction == nullptr)
                                    {
                                        routeFunction = route.find("", request.resource(), matches);
                                        if (routeFunction == nullptr)
                                        {
                                            response.status(Status::NotFound);
                                            return false;
                                        }
                                    }
                                    return true;
                                }
                            );

                            Response * finalResponse = &response;
                            bool errorOccured = false;

                            // Execute route function.
                            if (status == Priv::HttpReceiver::Status::Ok)
                            {
                                try
                                {
                                    routeFunction(request, response);
                                }
                                catch (std::exception &)
                                {
                                    response.status(Status::InternalServerError);
                                    status = Priv::HttpReceiver::Status::InternalError;
                                    errorOccured = true;
                                }
                                
                            }
                            // Client disconnected.
                            else if (status == Priv::HttpReceiver::Status::Disconnected)
                            {
                                return;
                            }
                            // Server or client error.
                            else
                            {
                                errorOccured = true;
                            }

                            // Execute error callback.
                            if (errorOccured)
                            {
                                errorResponse.status(response.status());
                                OnRequestRouter * errorOnRequest = nullptr;
                                OnRequestRouter::CallbackFunction * errorFunction = nullptr;

                                switch (status)
                                {
                                case Priv::HttpReceiver::Status::PeerError:
                                    errorOnRequest = &onClientError;
                                    break;
                                case Priv::HttpReceiver::Status::InternalError:
                                    errorOnRequest = &onServerError;
                                    break;  
                                default:
                                    break;
                                }

                                if (errorOnRequest != nullptr)
                                {
                                    if (errorOnRequest->callback)
                                    {
                                        errorFunction = &errorOnRequest->callback;
                                    }
                                    else if (onAnyError.callback)
                                    {
                                        errorFunction = &onAnyError.callback;
                                    }
                                }

                                if (errorFunction)
                                {
                                    try
                                    {
                                        (*errorFunction)(errorResponse);
                                    }
                                    catch (std::exception &)
                                    {
                                        errorResponse.clear();
                                        errorResponse.status(Status::InternalServerError);
                                        m_defaultOnError(errorResponse);
                                    }
                                }
                                else
                                {
                                    
                                    m_defaultOnError(errorResponse);
                                }

                                finalResponse = &errorResponse;
                            }

                            // Send final response to client.
                            sendResponse(*socket, *finalResponse);

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

        void Server::sendResponse(Socket::TcpSocket & socket, Response & response)
        {
            auto & headers = response.headers();
            auto & body = response.body();

            size_t estimatedHeadersSize = 64 * (headers.size() + 1);
            std::string respHeadData;
            respHeadData.reserve(estimatedHeadersSize);

            // Generate request line.          
            respHeadData =
                "HTTP/1.1 " + std::to_string(static_cast<unsigned long long>(response.status())) +
                " " + getStatusAsString(response.status()) + "\r\n";

            // Generate header lines.
            if (!headers.exists("server"))
            {
                respHeadData += "server: Wepp\r\n";
            }
            if (!headers.exists("connection"))
            {
                respHeadData += "connection: Closed\r\n";
            }
            headers.set("content-length", body.size());
            
            for (auto it = headers.begin(); it != headers.end(); it++)
            {
                respHeadData += it->first + ": " + it->second + "\r\n";
            }
            respHeadData += "\r\n";

            socket.send(respHeadData);

            // Send body data.
            const size_t bodySize = body.size();
            if (bodySize)
            {
                socket.send(body.data(), bodySize);
            }
        }

    }

}
