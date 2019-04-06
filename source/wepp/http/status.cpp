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

#include "wepp/http/status.hpp"

namespace Wepp
{

    namespace Http
    {

        static const std::string s_EmptyString = "";

        static const std::string s_Continue = "Continue";
        static const std::string s_SwitchingProtocols = "Switching Protocols";
        static const std::string s_Processing = "Processing";
        static const std::string s_EarlyHints = "Early Hints";

        static const std::string s_Ok = "OK";
        static const std::string s_Created = "Created";
        static const std::string s_Accepted = "Accepted";
        static const std::string s_NonAuthoritativeInformation = "Non-Authoritative Information";
        static const std::string s_NoContent= "No Content";
        static const std::string s_ResetContent = "Reset Content";
        static const std::string s_PartialContent = "Partial Content";
        static const std::string s_MultiStatus = "Multi-Status";
        static const std::string s_AlreadyReported = "Already Reported";
        static const std::string s_ImUsed = "I'm Used";
        
        static const std::string s_MultipleChoices = "Multiple Choices";
        static const std::string s_MovedPermanently = "Moved Permanently";
        static const std::string s_Found = "Found";
        static const std::string s_SeeOther = "See Other";
        static const std::string s_NotModified = "Not Modified";
        static const std::string s_UseProxy = "Use Proxy";
        static const std::string s_SwitchProxy = "Switch Proxy";
        static const std::string s_TemporaryRedirect = "Temporary Redirect";
        static const std::string s_PermanentRedirect = "Permanent Redirect";

        static const std::string s_BadRequest = "Bad Request";
        static const std::string s_Unauthorized = "Unauthorized";
        static const std::string s_PaymentRequired = "Payment Required";
        static const std::string s_Forbidden = "Forbidden";
        static const std::string s_NotFound = "Not Found";
        static const std::string s_MethodNotAllowed = "Method Not Allowed";
        static const std::string s_NotAcceptable = "Not Acceptable";
        static const std::string s_ProxyAuthenticationRequired = "Proxy Authentication Required";
        static const std::string s_RequestTimeout = "Request Timeout";
        static const std::string s_Conflict = "Conflict";
        static const std::string s_Gone = "Gone";
        static const std::string s_LengthRequired = "Length Required";
        static const std::string s_PreconditionFailed = "Precondition Failed";
        static const std::string s_PayloadTooLarge = "Payload Too Large";
        static const std::string s_UriTooLong = "Uri Too Long";
        static const std::string s_UnsupportedMediaType = "Unsupported Media Type";
        static const std::string s_RangeNotSatisfiable = "Range Not Satisfiable";
        static const std::string s_ExpectationFailed = "Expectation Failed";
        static const std::string s_ImATeapot = "I'm A Teapot";
        static const std::string s_MisdirectedRequest = "Misdirected Request";
        static const std::string s_UnprocessableEntity = "Unprocessable Entity";
        static const std::string s_Locked = "Locked";
        static const std::string s_FailedDependency = "Failed Dependency";
        static const std::string s_UpgradeRequired = "Upgrade Required";
        static const std::string s_PreconditionRequired = "Precondition Required";
        static const std::string s_TooManyRequests = "Too Many Requests";
        static const std::string s_RequestHeaderFieldsTooLarge = "Request Header Fields Too Large";
        static const std::string s_UnavailableForLegalReasons = "Unavailable For Legal Reasons";
            
        static const std::string s_InternalServerError = "Internal Server Error";
        static const std::string s_NotImplemented = "Not Implemented";
        static const std::string s_BadGateway = "Bad Gateway";
        static const std::string s_ServiceUnavailable = "Service Unavailable";
        static const std::string s_GatewayTimeout = "Gateway Timeout";
        static const std::string s_HttpVersionNotSupported = "Http Version Not Supported";
        static const std::string s_VariantAlsoNegotiates = "Variant Also Negotiates";
        static const std::string s_InsufficientStorage = "Insufficient Storage";
        static const std::string s_LoopDetected = "Loop Detected";
        static const std::string s_NotExtended = "Not Extended";
        static const std::string s_NetworkAuthenticationRequired = "Network Authentication Required";

            
        const std::string & getStatusAsString(const Status status)
        {

            switch (status)
            {
                case Status::Continue:                      return s_Continue; break;
                case Status::SwitchingProtocols:            return s_SwitchingProtocols; break;
                case Status::Processing:                    return s_Processing; break;
                case Status::EarlyHints:                    return s_EarlyHints; break;

                case Status::Ok:                            return s_Ok; break;
                case Status::Created:                       return s_Created; break;
                case Status::Accepted:                      return s_Accepted; break;
                case Status::NonAuthoritativeInformation:   return s_NonAuthoritativeInformation; break;
                case Status::NoContent:                     return s_NoContent; break;
                case Status::ResetContent:                  return s_ResetContent; break;
                case Status::PartialContent:                return s_PartialContent; break;
                case Status::MultiStatus:                   return s_MultiStatus; break;
                case Status::AlreadyReported:               return s_AlreadyReported; break;
                case Status::ImUsed:                        return s_ImUsed; break;
 
                case Status::MultipleChoices:               return s_MultipleChoices; break;
                case Status::MovedPermanently:              return s_MovedPermanently; break;
                case Status::Found:                         return s_Found; break;
                case Status::SeeOther:                      return s_SeeOther; break;
                case Status::NotModified:                   return s_NotModified; break;
                case Status::UseProxy:                      return s_UseProxy; break;
                case Status::SwitchProxy:                   return s_SwitchProxy; break;
                case Status::TemporaryRedirect:             return s_TemporaryRedirect; break;
                case Status::PermanentRedirect:             return s_PermanentRedirect; break;

                case Status::BadRequest:                    return s_BadRequest; break;
                case Status::Unauthorized:                  return s_Unauthorized; break;
                case Status::PaymentRequired:               return s_PaymentRequired; break;
                case Status::Forbidden:                     return s_Forbidden; break;
                case Status::NotFound:                      return s_NotFound; break;
                case Status::MethodNotAllowed:              return s_MethodNotAllowed; break;
                case Status::NotAcceptable:                 return s_NotAcceptable; break;
                case Status::ProxyAuthenticationRequired:   return s_ProxyAuthenticationRequired; break;
                case Status::RequestTimeout:                return s_RequestTimeout; break;
                case Status::Conflict:                      return s_Conflict; break;
                case Status::Gone:                          return s_Gone; break;
                case Status::LengthRequired:                return s_LengthRequired; break;
                case Status::PreconditionFailed:            return s_PreconditionFailed; break;
                case Status::PayloadTooLarge:               return s_PayloadTooLarge; break;
                case Status::UriTooLong:                    return s_UriTooLong; break;
                case Status::UnsupportedMediaType:          return s_UnsupportedMediaType; break;
                case Status::RangeNotSatisfiable:           return s_RangeNotSatisfiable; break;
                case Status::ExpectationFailed:             return s_ExpectationFailed; break;
                case Status::ImATeapot:                     return s_ImATeapot; break;
                case Status::MisdirectedRequest:            return s_MisdirectedRequest; break;
                case Status::UnprocessableEntity:           return s_UnprocessableEntity; break;
                case Status::Locked:                        return s_Locked; break;
                case Status::FailedDependency:              return s_FailedDependency; break;
                case Status::UpgradeRequired:               return s_UpgradeRequired; break;
                case Status::PreconditionRequired:          return s_PreconditionRequired; break;
                case Status::TooManyRequests:               return s_TooManyRequests; break;
                case Status::RequestHeaderFieldsTooLarge:   return s_RequestHeaderFieldsTooLarge; break;
                case Status::UnavailableForLegalReasons:    return s_UnavailableForLegalReasons; break;

                case Status::InternalServerError:           return s_InternalServerError; break;
                case Status::NotImplemented:                return s_NotImplemented; break;
                case Status::BadGateway:                    return s_BadGateway; break;
                case Status::ServiceUnavailable:            return s_ServiceUnavailable; break;
                case Status::GatewayTimeout:                return s_GatewayTimeout; break;
                case Status::HttpVersionNotSupported:       return s_HttpVersionNotSupported; break;
                case Status::VariantAlsoNegotiates:         return s_VariantAlsoNegotiates; break;
                case Status::InsufficientStorage:           return s_InsufficientStorage; break;
                case Status::LoopDetected:                  return s_LoopDetected; break;
                case Status::NotExtended:                   return s_NotExtended; break;
                case Status::NetworkAuthenticationRequired: return s_NetworkAuthenticationRequired; break;
                default:    break;
            }

            return s_EmptyString;
        }

        bool isInformationalStatus(const Status status)
        {
            const uint32_t code = static_cast<uint32_t>(status);
            return code >= 100 && code < 200;
        }

        bool isSuccessStatus(const Status status)
        {
            const uint32_t code = static_cast<uint32_t>(status);
            return code >= 200 && code < 300;
        }

        bool isRedirectionStatus(const Status status)
        {
            const uint32_t code = static_cast<uint32_t>(status);
            return code >= 300 && code < 400;
        }

        bool isClientErrorStatus(const Status status)
        {
            const uint32_t code = static_cast<uint32_t>(status);
            return code >= 400 && code < 500;
        }

        bool isServerErrorStatus(const Status status)
        {
            const uint32_t code = static_cast<uint32_t>(status);
            return code >= 500 && code < 600;
        }

    }

}