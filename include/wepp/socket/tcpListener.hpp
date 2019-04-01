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

#ifndef WEPP_SOCKET_TCP_LISTENER_HPP
#define WEPP_SOCKET_TCP_LISTENER_HPP

#include "wepp/build.hpp"
#include "wepp/socket/tcpSocket.hpp"
#include "wepp/task.hpp"
#include "wepp/semaphore.hpp"
#include <thread>
#include <mutex>
#include <queue>
#include <string>

/**
* Wepp namespace.
*
*/
namespace Wepp
{

    /**
    * Wepp namespace.
    *
    */
    namespace Socket
    {

        /**
        * Tcp listener class.
        *
        */
        class WEPP_API TcpListener : Socket
        {

        public:

            /**
            * Constructor.
            *
            */
            TcpListener();

            /**
            * Virtual destructor.
            *
            */
            ~TcpListener();

            /**
            * Deleted copy constructor.
            *
            */
            TcpListener(const TcpListener &) = delete;

            /**
            * Start the listening thread.
            *
            * @param[in] address - Endpoint port.
            * @param[in] endpoint - Endpoint address.
            *
            * @return Task. Call wait() on the task to wait for the listener complete the startup.
            *
            * @remark Endpoint and internal thread is started, but not listening until calling listen().
            *
            * @see listen
            *
            */
            Task<> start(const uint16_t port, const std::string endpoint = "");

            /**
            * Stop listening.
            *
            * @return Task. Call wait() on the task to wait for the listener complete the shutdown.
            *
            */
            Task<> stop();

            /**
            * Listen for incoming connections.
            *
            * @return Task containg a tcp socket. Call wait() on the task to wait for incoming connection.
            *
            */
            Task<std::shared_ptr<TcpSocket>> listen();

        private:

            /**
            * Function for cleaning up before stopping.
            *
            */
            void handleStop();

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

            typedef TaskController<std::shared_ptr<TcpSocket> > ListenTaskController;    /**< Typedef of listen task controller.*/

            std::thread                         m_thread;       /**< Main thread. */
            std::mutex                          m_mutex;        /**< Mutex lock for multiple methods.*/
            std::atomic<State>                  m_state;        /**< Current state of listener. */
            TaskController<>                    m_startTask;    /**< Task for starting the listener. */
            TaskController<>                    m_stopTask;     /**< Task for stopping the listener. */
            Semaphore                           m_sempahore;    /**< Sempahore, triggering thread to listen.*/
            std::queue<ListenTaskController>    m_listenQueue;  /**< Queue of listen tasks.*/

        };

    }

}

#endif
