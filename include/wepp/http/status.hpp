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

#ifndef WEPP_HTTP_STATUS_HPP
#define WEPP_HTTP_STATUS_HPP

#include "wepp/build.hpp"
#include <string>

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
        * Status enumerator.
        *
        *
        */
        enum class Status: uint32_t
        {
            // Informational.
            Continue                        = 100,
            SwitchingProtocols              = 101,
            Processing                      = 102,
            EarlyHints                      = 103,

            // Success.
            Ok                              = 200,
            Created                         = 201,
            Accepted                        = 202,
            NonAuthoritativeInformation     = 203,
            NoContent                       = 204,
            ResetContent                    = 205,
            PartialContent                  = 206,
            MultiStatus                     = 207,
            AlreadyReported                 = 208,
            ImUsed                          = 226,

            // Redirection.
            MultipleChoices                 = 300,
            MovedPermanently                = 301,
            Found                           = 302,
            SeeOther                        = 303,
            NotModified                     = 304,
            UseProxy                        = 305,
            SwitchProxy                     = 306,
            TemporaryRedirect               = 307,
            PermanentRedirect               = 308,

            // Client error.
            BadRequest                      = 400,
            Unauthorized                    = 401,
            PaymentRequired                 = 402,
            Forbidden                       = 403,
            NotFound                        = 404,
            MethodNotAllowed                = 405,
            NotAcceptable                   = 406,
            ProxyAuthenticationRequired     = 407,
            RequestTimeout                  = 408,
            Conflict                        = 409,
            Gone                            = 410,
            LengthRequired                  = 411,
            PreconditionFailed              = 412,
            PayloadTooLarge                 = 413,
            UriTooLong                      = 414,
            UnsupportedMediaType            = 415,
            RangeNotSatisfiable             = 416,
            ExpectationFailed               = 417,
            ImATeapot                       = 418,
            MisdirectedRequest              = 421,
            UnprocessableEntity             = 422,
            Locked                          = 423,
            FailedDependency                = 424,
            UpgradeRequired                 = 426,
            PreconditionRequired            = 428,
            TooManyRequests                 = 429,
            RequestHeaderFieldsTooLarge     = 431,
            UnavailableForLegalReasons      = 451,

            // Server error.
            InternalServerError             = 500,
            NotImplemented                  = 501,
            BadGateway                      = 502,
            ServiceUnavailable              = 503,
            GatewayTimeout                  = 504,
            HttpVersionNotSupported         = 505,
            VariantAlsoNegotiates           = 506,
            InsufficientStorage             = 507,
            LoopDetected                    = 508,
            NotExtended                     = 510,
            NetworkAuthenticationRequired   = 511
        };

        /**
        * Get name of method as string.
        *
        * @return Name of method given as parameter.
        *
        */
        WEPP_API const std::string & getStatusAsString(const Status status);

        /**
        * Checks if status is an informational status.
        *
        * @return True if passed status is an informational status(1xx status code), else false.
        *
        */
        WEPP_API bool isInformationalStatus(const Status status);

        /**
        * Checks if status is a success status.
        *
        * @return True if passed status is an success status(2xx status code), else false.
        *
        */
        WEPP_API bool isSuccessStatus(const Status status);

        /**
        * Checks if status is an redirection status.
        *
        * @return True if passed status is a redirection status(3xx status code), else false.
        *
        */
        WEPP_API bool isRedirectionStatus(const Status status);

        /**
        * Checks if status is a client error status.
        *
        * @return True if passed status is client error status(4xx status code), else false.
        *
        */
        WEPP_API bool isClientErrorStatus(const Status status);

        /**
        * Checks if status is a server error status.
        *
        * @return True if passed status is server error status(5xx status code), else false.
        *
        */
        WEPP_API bool isServerErrorStatus(const Status status);

    }

}

#endif

/**
* @file
*
* This header file define the interfaces of http status codes.
*
*/