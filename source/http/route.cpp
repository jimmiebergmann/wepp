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

#include "wepp/http/route.hpp"
#include <algorithm>

namespace Wepp
{

    namespace Http
    {

        // Route class.
        Route::Route()
        { }

        Route::~Route()
        {
            for (auto it = m_methods.begin(); it != m_methods.end(); it++)
            {
                delete it->second;
            }
        }

        size_t Route::methodCount() const
        {
            return m_methods.size();
        }

        RouteMethod & Route::operator[](const Method method)
        {
            return this->operator[](getMethodName(method).c_str());
        }

        RouteMethod & Route::operator[](const std::string & method)
        {
            std::string methodName = method;
            std::transform(methodName.begin(), methodName.end(), methodName.begin(), ::toupper);

            auto it = m_methods.find(methodName);
            if (it != m_methods.end())
            {
                return *(it->second);
            }

            RouteMethod * routeMethod = new RouteMethod(methodName);
            m_methods.insert({ methodName, routeMethod });
            return *routeMethod;
        }


        // Route method class.
        RouteMethod::RouteMethod(const std::string & name) :
            m_name(name)
        { }

        const std::string & RouteMethod::name() const
        {
            return m_name;
        }

       /* RoutePath & RouteMethod::operator[](const std::string & path)
        {

        }*/


        // Route path class.
        RoutePath::RoutePath(Route::CallbackFunc callback) :
            callback(callback)
        { }
       
    }

}