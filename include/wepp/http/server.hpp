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

#ifndef WEPP_HTTP_SERVER_HPP
#define WEPP_HTTP_SERVER_HPP

#include "wepp/build.hpp"
#include "wepp/http/router.hpp"
#include "wepp/task.hpp"
#include "wepp/priv/receivePool.hpp"
#include "wepp/socket/tcpListener.hpp"
#include <thread>
#include <mutex>
#include <queue>

/**
* Wepp namespace.
*
*/
namespace Wepp
{

    /**
    * Http namespace.
    *
    */
    namespace Http
    {

        /**
        * Http server.
        *
        *
        */
        class WEPP_API Server
        {

        public:

            /**
            * Constuctor.
            *
            */
            Server();

            /**
            * Destructor.
            *
            */
            ~Server();

            /**
            * Deleted copy constructor.
            *
            */
            Server(const Server &) = delete;

            /**
            * Asynchronous function for starting the server.
            *
            */
            Task<> start(const uint16_t port, const std::string & endpoint = "");

            /**
            * Asynchronous function for stopping the server.
            *
            */
            Task<> stop();

            /**
            * Asynchronous function for checking if server is stopped.
            *
            */
            Task<> stopped();

            /**
            * Class containing all the routes.
            *
            * @remark Do not change any routes when the server has been started via start().
            *
            */
            Router route;

            /**
            * Callback function triggered if any error occured while receiving client request,
            * and related on...Error function is not set.
            *
            * @remark The callback function is not triggered if the request is routed
            *         and successfully executed, regardless of status code set by route implementer.
            *
            */
            OnRequestRouter onAnyError;

            /**
            * Callback function triggered if a client error occuered while receiving client request.
            *
            * @remark The callback function is not triggered if the request is routed
            *         and successfully executed, regardless of status code set by route implementer.
            *
            */
            OnRequestRouter onClientError;

            /**
            * Callback function triggered if a server error occuered while receiving client request.
            *
            * @remark The callback function is not triggered if the request is routed
            *         and successfully executed, regardless of status code set by route implementer.
            *
            */
            OnRequestRouter onServerError;

        private:

            /**
            * Function for cleaning up before stopping.
            *
            */
            void handleStop();

            /**
            * Send response to client.
            *
            * @param socket     - Tcp socket of client.
            * @param response   - Response class, containing all the information needed for construction a HTTP response.
            *
            */
            void sendResponse(Socket::TcpSocket & socket, Response & response);

            /**
            * Enumerator of different server states.
            *
            */
            enum class State
            {
                Stopped,
                Stopping,
                Started,
                Starting
            };

            std::thread m_thread;               /**< Main thread. */
            std::mutex m_mutex;                 /**< Mutex lock for multiple methods.*/
            std::atomic<State> m_state;         /**< Current state of server. */
            TaskController<> m_startTask;       /**< Task for starting the server. */
            TaskController<> m_stopTask;        /**< Task for stopping the server. */    
            Socket::TcpListener m_listener;     /**< Tcp listener. */
            Priv::ReceivePool m_receivePool;    /**< Thred pool of receive pool workers.*/

            OnRequestRouter::CallbackFunction m_defaultOnError; /**< Default response function of errors. */

        };

    }

}

#endif
