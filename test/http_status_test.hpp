#include "gtest/gtest.h"
#include "wepp/http/status.hpp"

using namespace Wepp;

TEST(Http_Status, getStatusAsString)
{
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Continue).c_str(), "Continue");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::SwitchingProtocols).c_str(), "Switching Protocols");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Processing).c_str(), "Processing");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::EarlyHints).c_str(), "Early Hints");

    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Ok).c_str(), "OK");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Created).c_str(), "Created");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Accepted).c_str(), "Accepted");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::NonAuthoritativeInformation).c_str(), "Non-Authoritative Information");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::NoContent).c_str(), "No Content");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::ResetContent).c_str(), "Reset Content");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::PartialContent).c_str(), "Partial Content");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::MultiStatus).c_str(), "Multi-Status");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::AlreadyReported).c_str(), "Already Reported");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::ImUsed).c_str(), "I'm Used");

    EXPECT_STREQ(Http::getStatusAsString(Http::Status::MultipleChoices).c_str(), "Multiple Choices");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::MovedPermanently).c_str(), "Moved Permanently");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Found).c_str(), "Found");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::SeeOther).c_str(), "See Other");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::NotModified).c_str(), "Not Modified");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::UseProxy).c_str(), "Use Proxy");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::SwitchProxy).c_str(), "Switch Proxy");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::TemporaryRedirect).c_str(), "Temporary Redirect");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::PermanentRedirect).c_str(), "Permanent Redirect");

    EXPECT_STREQ(Http::getStatusAsString(Http::Status::BadRequest).c_str(), "Bad Request");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Unauthorized).c_str(), "Unauthorized");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::PaymentRequired).c_str(), "Payment Required");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Forbidden).c_str(), "Forbidden");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::NotFound).c_str(), "Not Found");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::MethodNotAllowed).c_str(), "Method Not Allowed");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::NotAcceptable).c_str(), "Not Acceptable");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::ProxyAuthenticationRequired).c_str(), "Proxy Authentication Required");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::RequestTimeout).c_str(), "Request Timeout");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Conflict).c_str(), "Conflict");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Gone).c_str(), "Gone");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::LengthRequired).c_str(), "Length Required");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::PreconditionFailed).c_str(), "Precondition Failed");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::PayloadTooLarge).c_str(), "Payload Too Large");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::UriTooLong).c_str(), "Uri Too Long");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::UnsupportedMediaType).c_str(), "Unsupported Media Type");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::RangeNotSatisfiable).c_str(), "Range Not Satisfiable");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::ExpectationFailed).c_str(), "Expectation Failed");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::ImATeapot).c_str(), "I'm A Teapot");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::MisdirectedRequest).c_str(), "Misdirected Request");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::UnprocessableEntity).c_str(), "Unprocessable Entity");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::Locked).c_str(), "Locked");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::FailedDependency).c_str(), "Failed Dependency");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::UpgradeRequired).c_str(), "Upgrade Required");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::PreconditionRequired).c_str(), "Precondition Required");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::TooManyRequests).c_str(), "Too Many Requests");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::RequestHeaderFieldsTooLarge).c_str(), "Request Header Fields Too Large");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::UnavailableForLegalReasons).c_str(), "Unavailable For Legal Reasons");

    EXPECT_STREQ(Http::getStatusAsString(Http::Status::InternalServerError).c_str(), "Internal Server Error");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::NotImplemented).c_str(), "Not Implemented");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::BadGateway).c_str(), "Bad Gateway");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::ServiceUnavailable).c_str(), "Service Unavailable");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::GatewayTimeout).c_str(), "Gateway Timeout");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::HttpVersionNotSupported).c_str(), "Http Version Not Supported");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::VariantAlsoNegotiates).c_str(), "Variant Also Negotiates");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::InsufficientStorage).c_str(), "Insufficient Storage");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::LoopDetected).c_str(), "Loop Detected");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::NotExtended).c_str(), "Not Extended");
    EXPECT_STREQ(Http::getStatusAsString(Http::Status::NetworkAuthenticationRequired).c_str(), "Network Authentication Required");
}