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

#include "wepp/http/router.hpp"
#include <algorithm>
#include <regex>
#include <set>
#include <memory>

namespace Wepp
{

    namespace Http
    {

        // Route class.
        Router::Router()
        { }

        Router::~Router()
        {
            for (auto it = m_methods.begin(); it != m_methods.end(); it++)
            {
                delete it->second;
            }
        }

        size_t Router::methodCount() const
        {
            return m_methods.size();
        }

        RouteMethod & Router::operator[](const Method method)
        {
            return this->operator[](getMethodName(method).c_str());
        }

        RouteMethod & Router::operator[](const std::string & method)
        {
            std::string methodName = method;
            std::transform(methodName.begin(), methodName.end(), methodName.begin(), ::toupper);

            auto it = m_methods.find(methodName);
            if (it != m_methods.end())
            {
                return *it->second;
            }

            RouteMethod * routeMethod = new RouteMethod(methodName);
            auto result = m_methods.insert({ methodName, routeMethod });
            return *routeMethod;
        }


        // Route method class.
        // Tag.
        RouteMethod::Tag::Tag(const std::string & key, const std::string & regex) :
            key(key),
            regex(regex)
        { }

        RouteMethod::~RouteMethod()
        {
        }

        RouteMethod::RouteMethod(const std::string & name) :
            m_name(name)
        { }

        const std::string & RouteMethod::name() const
        {
            return m_name;
        }

        RouteCallback & RouteMethod::operator[](const std::string & path)
        {
            std::vector<std::string> dirs;
            std::regex findDirRegex(R"((?:/?((?:[^/]*<[^>]*>)*)?([^/]*)))");
            std::regex findTagRegex(R"(<[^>]*>)");
            std::regex findTagDataRegex(R"((?:<([^>]*)>))");
            std::set<std::string> usedTagKeys;

            const char * iter = path.c_str();
            for (std::cmatch m; *iter && std::regex_search(iter, m, findDirRegex); iter += m[0].length())
            {
                dirs.push_back(m[1].length() ? m[1] : m[2]);
            }
            
            RouteNode * currentNode = &m_rootNode;
            for (auto const & dir : dirs)
            {
                auto it = currentNode->regularTree.find(dir);
                if (it != currentNode->regularTree.end())
                {
                    // Found regular node, move to next dir.
                    currentNode = it->second;
                    continue;
                }

                it = currentNode->tagTree.find(dir);
                if (it != currentNode->tagTree.end())
                {
                    // Found tag key node, move to next dir.
                    currentNode = it->second;
                    continue;
                }

                // Insert new node, but first check if it's a regular or a tag node.
                if (std::regex_search(dir, findTagRegex))
                {
                    // Insert tag node.
                    RouteNode * newRouteNode = new RouteNode();
                    currentNode->tagTree.insert({ dir, newRouteNode });
                    currentNode = newRouteNode;

                    // Find data in each tag.
                    const char * tagIter = dir.c_str();
                    for (std::cmatch m; *tagIter && std::regex_search(tagIter, m, findTagDataRegex); tagIter += m[0].length())
                    {
                        std::string tagKey = m[1];
                        std::string tagRegex = "";

                        size_t tagRegexPos = tagKey.find_first_of(',');

                        if (tagRegexPos != std::string::npos)
                        {
                            tagRegex = tagKey.substr(tagRegexPos + 1);
                            tagKey.resize(tagRegexPos);
                        }

                        // Make sure the tag names are unique.
                        auto usedTagIt = usedTagKeys.find(tagKey);
                        if (usedTagIt != usedTagKeys.end())
                        {
                            throw std::runtime_error("Name of tag \"" + tagKey + "\" has already been routed.");
                        }
                        usedTagKeys.insert(tagKey);

                        Tag * newTag = new Tag(tagKey, tagRegex);
                        newRouteNode->tags.push_back(newTag); 
                    }

                    continue;
                }

                // Insert regular node.
                RouteNode * newRouteNode = new RouteNode();
                currentNode->regularTree.insert({ dir, newRouteNode});
                currentNode = newRouteNode;
            }

            if (!currentNode->routeCallback)
            {
                currentNode->routeCallback = new RouteCallback();
            }

            return *(currentNode->routeCallback);
        }

        const Router::CallbackFunc & RouteMethod::find(const std::string & path, std::vector<std::reference_wrapper<const Tag>> & tags) const
        {
            static const Router::CallbackFunc s_defaultCallbackFunc = nullptr;

            tags.clear();

            // Validate path.
            /*std::regex validPathRegex(R"(^[^<>]*)");
            if (!std::regex_search(path, validPathRegex))
            {
                return s_defaultCallbackFunc;
            }*/

            std::vector<std::string> dirs;
            std::regex findDirRegex(R"((?:/?([^/]*)))");

            const char * iter = path.c_str();
            for (std::cmatch m; *iter && std::regex_search(iter, m, findDirRegex); iter += m[0].length())
            {
                dirs.push_back(m[1]);
            }

            const RouteNode * currentNode = &m_rootNode;
            const RouteCallback * currentRouteCallback = currentNode->routeCallback;
            for (auto const & dir : dirs)
            {
                auto it = currentNode->regularTree.find(dir);

                // Find regular dir and more to next one.
                if (it != currentNode->regularTree.end())
                {
                    currentNode = it->second;
                    currentRouteCallback = it->second->routeCallback;
                    continue;
                }

                // Find tag node.

                // Failed to find any more route.
                return s_defaultCallbackFunc;
            }

            if (currentRouteCallback)
            {
                return currentRouteCallback->callback;
            }
            return s_defaultCallbackFunc;
        }

        // Route node.
        RouteMethod::RouteNode::RouteNode() :
            routeCallback(nullptr)
        { }

        RouteMethod::RouteNode::~RouteNode()
        {
            for (auto it = regularTree.begin(); it != regularTree.end(); it++)
            {
                delete it->second;
            }

            for (auto it = tagTree.begin(); it != tagTree.end(); it++)
            {
                delete it->second;
            }

            for (auto it = tags.begin(); it != tags.end(); it++)
            {
                delete *it;
            }

            if (routeCallback != nullptr)
            {
                delete routeCallback;
            }
        }


        // Route path class.
        RouteCallback::RouteCallback()
        { }

        RouteCallback & RouteCallback::operator=(const Router::CallbackFunc & p_callback)
        {
            callback = p_callback;
            return *this;
        }

        /*const Router::CallbackFunc & RouteCallback::callback() const
        {
            return m_callback;
        }*/
       
    }

}