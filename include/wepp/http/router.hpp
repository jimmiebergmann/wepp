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
#include <functional>
#include <vector>

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
        class RouteCallback;

        /**
        * Http routing class.
        *
        *
        */
        class WEPP_API Router
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
            Router();

            /**
            * Destructor.
            *
            */
            ~Router();

            /**
            * Get number of routed methods.
            *
            */
            size_t methodCount() const;

            /**
            * Gets existing route method, or creates a new empty one if method does not exist in the method map.
            *
            * @param[in] method - Enumerator representing method.
            *
            */
            RouteMethod & operator[](const Method method);

            /**
            * Gets existing route method, or creates a new empty one if method does not exist in the method map.
            *
            * @param[in] method - Name of method to route.
            *
            */
            RouteMethod & operator[](const std::string & method);

        private:
            
            /**
            * Deleted copy constructor.
            *
            */
            Router(const Router &) = delete;

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
            * Structure describing tag information.
            *
            */
            struct Tag
            {
                Tag(const std::string & key, const std::string & regex);

                std::string key;
                std::string regex;
            };

            /**
            * Destructor.
            *
            */
            ~RouteMethod();

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
            * @remarks Directories and regex cannot constist of the characters  '<' and '>'.
            *
            */
            RouteCallback & operator[](const std::string & path);

            /**
            * Find callback function by path.
            *
            * @param[in] path - Path to find in route tree.
            * @param[out] tags - Vector of matching tags.
            *
            * @return Found function.
            *
            */
            const Router::CallbackFunc & find(const std::string & path, std::vector<std::string> & matches) const;

        private:

            /**
            * Deleted copy constructor.
            *
            */
            RouteMethod(const RouteMethod &) = delete;

            struct RouteNode;
            typedef std::map<std::string, RouteNode*> RouteTree;  /**< Route regular tree map typedef. */


            /**
            * Structure describing a node of the route tree.
            *
            */
            struct RouteNode
            {
                RouteNode();
                ~RouteNode();

                RouteTree regularTree;
                RouteTree tagTree;
                RouteCallback * routeCallback;
                std::string dir;
            };


            std::string m_name;     /**< Name of method. */
            RouteNode m_rootNode;   /**< Root note of route path tree. */

        };

        /**
        * Http path routing class.
        *
        *
        */
        class WEPP_API RouteCallback
        {

        public:

            /**
            * Constructor.

            *
            */
            RouteCallback();

            /**
            * Assigning callback function.
            *
            */
            RouteCallback & operator=(const Router::CallbackFunc & callback);

            Router::CallbackFunc callback; /**< Callback function. */

        private:

            /**
            * Deleted copy constructor.
            *
            */
            RouteCallback(const RouteCallback &) = delete;

        };

    }

}

#endif