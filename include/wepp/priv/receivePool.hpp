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

#ifndef WEPP_PRIV_RECEIVE_POOL_HPP
#define WEPP_PRIV_RECEIVE_POOL_HPP

#include "wepp/build.hpp"
#include "wepp/priv/threadPool.hpp"
#include "wepp/socket/tcpSocket.hpp"
#include "wepp/priv/httpReceiver.hpp"
/*#include "wepp/task.hpp"
#include "wepp/semaphore.hpp"
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <set>
#include <functional>
#include <exception>
#include <tuple>
*/

/**
* Wepp namespace.
*
*/
namespace Wepp
{

    /**
    * Private namespace.
    *
    */
    namespace  Priv
    {
        template<typename ... Args>
        class ReceivePoolWorker : public ThreadWorker<Args...>
        {

        public:

            typedef std::function<void(HttpReceiver &, std::shared_ptr<Socket::TcpSocket>)> ExecutionFunction;

            /**
            * Constructor.
            *
            */
            ReceivePoolWorker(ThreadPoolBase<Args...> * pool, ExecutionFunction executionFunction);

            /**
            * Executes the function passed to constructor.
            *
            */
            virtual void execute(std::shared_ptr<Socket::TcpSocket> socket);

        private:

            ExecutionFunction   m_function;
            HttpReceiver        m_receiver;

        };

        typedef ThreadPool<ReceivePoolWorker<std::shared_ptr<Socket::TcpSocket>>, std::shared_ptr<Socket::TcpSocket>> ReceivePool;

    }

}


#include "wepp/priv/receivePool.inl"

#endif