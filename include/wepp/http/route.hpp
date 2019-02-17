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

#ifndef WEPP_HTTP_ROUTE_HPP
#define WEPP_HTTP_ROUTE_HPP

#include "wepp/build.hpp"
#include "wepp/http/method.hpp"
#include "wepp/http/request.hpp"
#include "wepp/http/response.hpp"
#include <string>
#include <map>
#include <memory>
#include <functional>

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

        class RouteMethod;
        class RoutePath;

        /**
        * Http routing class.
        *
        *
        */
        class WEPP_API Route
        {

        public:

            /**
            * Callback function, called when successfully routed.
            *
            */
            typedef std::function<void(const Request &, Response &)> CallbackFunc;

            /**
            * Constuctor.
            *
            */
            Route();

            /**
            * Destructor.
            *
            */
            ~Route();

            /**
            * Get number of routed methods.
            *
            */
            size_t methodCount() const;

            /**
            * Gets existing route method, or creates a new empty one if method does not exist in the method map.
            *
            */
            RouteMethod & operator[](const Method method);
            RouteMethod & operator[](const std::string & method);

        private:
            
            /**
            * Deleted copy constructor.
            *
            */
            Route(const Route &) = delete;

            std::map<std::string, RouteMethod*> m_methods; /**< Map of routed methods. */

        };

        /**
        * Http method routing class.
        *
        *
        */
        class WEPP_API RouteMethod
        {

        public:

            /**
            * Constructing method routing by name.
            *
            */
            RouteMethod(const std::string & name);

            /**
            * Get name of method.
            *
            */
            const std::string & name() const;

            /**
            * Gets existing route path, or creates a new empty one if path does not exist in the path map.
            *
            */
            //RoutePath & operator[](const std::string & path);

        private:

            /**
            * Deleted copy constructor.
            *
            */
            RouteMethod(const RouteMethod &) = delete;

            std::string m_name; /**< Name of method. */

        };

        /**
        * Http path routing class.
        *
        *
        */
        class WEPP_API RoutePath
        {

        public:

            /**
            * Constructing path routing by routing callback function.
            *
            * @param[in] callback - Routing callback function.
            *
            */
            RoutePath(Route::CallbackFunc callback);

            const Route::CallbackFunc callback; /**< Callback function. */

        private:

            /**
            * Deleted copy constructor.
            *
            */
            RoutePath(const RoutePath &) = delete;

        };



    }

}

#endif