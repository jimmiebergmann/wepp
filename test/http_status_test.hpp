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

TEST(Http_Status, isStatus)
{
    {
        EXPECT_TRUE(Http::isInformationalStatus(Http::Status::Continue));
        EXPECT_TRUE(Http::isInformationalStatus(Http::Status::SwitchingProtocols));
        EXPECT_TRUE(Http::isInformationalStatus(Http::Status::Processing));
        EXPECT_TRUE(Http::isInformationalStatus(Http::Status::EarlyHints));

        EXPECT_TRUE(Http::isSuccessStatus(Http::Status::Ok));
        EXPECT_TRUE(Http::isSuccessStatus(Http::Status::Created));
        EXPECT_TRUE(Http::isSuccessStatus(Http::Status::Accepted));
        EXPECT_TRUE(Http::isSuccessStatus(Http::Status::NonAuthoritativeInformation));
        EXPECT_TRUE(Http::isSuccessStatus(Http::Status::NoContent));
        EXPECT_TRUE(Http::isSuccessStatus(Http::Status::ResetContent));
        EXPECT_TRUE(Http::isSuccessStatus(Http::Status::PartialContent));
        EXPECT_TRUE(Http::isSuccessStatus(Http::Status::MultiStatus));
        EXPECT_TRUE(Http::isSuccessStatus(Http::Status::AlreadyReported));
        EXPECT_TRUE(Http::isSuccessStatus(Http::Status::ImUsed));

        EXPECT_TRUE(Http::isRedirectionStatus(Http::Status::MultipleChoices));
        EXPECT_TRUE(Http::isRedirectionStatus(Http::Status::MovedPermanently));
        EXPECT_TRUE(Http::isRedirectionStatus(Http::Status::Found));
        EXPECT_TRUE(Http::isRedirectionStatus(Http::Status::SeeOther));
        EXPECT_TRUE(Http::isRedirectionStatus(Http::Status::NotModified));
        EXPECT_TRUE(Http::isRedirectionStatus(Http::Status::UseProxy));
        EXPECT_TRUE(Http::isRedirectionStatus(Http::Status::SwitchProxy));
        EXPECT_TRUE(Http::isRedirectionStatus(Http::Status::TemporaryRedirect));
        EXPECT_TRUE(Http::isRedirectionStatus(Http::Status::PermanentRedirect));

        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::BadRequest));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::Unauthorized));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::PaymentRequired));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::Forbidden));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::NotFound));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::MethodNotAllowed));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::NotAcceptable));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::ProxyAuthenticationRequired));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::RequestTimeout));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::Conflict));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::Gone));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::LengthRequired));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::PreconditionFailed));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::PayloadTooLarge));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::UriTooLong));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::UnsupportedMediaType));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::RangeNotSatisfiable));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::ExpectationFailed));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::ImATeapot));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::MisdirectedRequest));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::UnprocessableEntity));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::Locked));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::FailedDependency));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::UpgradeRequired));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::PreconditionRequired));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::TooManyRequests));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::RequestHeaderFieldsTooLarge));
        EXPECT_TRUE(Http::isClientErrorStatus(Http::Status::UnavailableForLegalReasons));

        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::InternalServerError));
        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::NotImplemented));
        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::BadGateway));
        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::ServiceUnavailable));
        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::GatewayTimeout));
        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::HttpVersionNotSupported));
        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::VariantAlsoNegotiates));
        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::InsufficientStorage));
        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::LoopDetected));
        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::NotExtended));
        EXPECT_TRUE(Http::isServerErrorStatus(Http::Status::NetworkAuthenticationRequired));
    }
    {
        EXPECT_FALSE(Http::isServerErrorStatus(Http::Status::Continue));
        EXPECT_FALSE(Http::isServerErrorStatus(Http::Status::SwitchingProtocols));
        EXPECT_FALSE(Http::isServerErrorStatus(Http::Status::Processing));
        EXPECT_FALSE(Http::isServerErrorStatus(Http::Status::EarlyHints));

        EXPECT_FALSE(Http::isInformationalStatus(Http::Status::Ok));
        EXPECT_FALSE(Http::isInformationalStatus(Http::Status::Created));
        EXPECT_FALSE(Http::isInformationalStatus(Http::Status::Accepted));
        EXPECT_FALSE(Http::isInformationalStatus(Http::Status::NonAuthoritativeInformation));
        EXPECT_FALSE(Http::isInformationalStatus(Http::Status::NoContent));
        EXPECT_FALSE(Http::isInformationalStatus(Http::Status::ResetContent));
        EXPECT_FALSE(Http::isInformationalStatus(Http::Status::PartialContent));
        EXPECT_FALSE(Http::isInformationalStatus(Http::Status::MultiStatus));
        EXPECT_FALSE(Http::isInformationalStatus(Http::Status::AlreadyReported));
        EXPECT_FALSE(Http::isInformationalStatus(Http::Status::ImUsed));

        EXPECT_FALSE(Http::isSuccessStatus(Http::Status::MultipleChoices));
        EXPECT_FALSE(Http::isSuccessStatus(Http::Status::MovedPermanently));
        EXPECT_FALSE(Http::isSuccessStatus(Http::Status::Found));
        EXPECT_FALSE(Http::isSuccessStatus(Http::Status::SeeOther));
        EXPECT_FALSE(Http::isSuccessStatus(Http::Status::NotModified));
        EXPECT_FALSE(Http::isSuccessStatus(Http::Status::UseProxy));
        EXPECT_FALSE(Http::isSuccessStatus(Http::Status::SwitchProxy));
        EXPECT_FALSE(Http::isSuccessStatus(Http::Status::TemporaryRedirect));
        EXPECT_FALSE(Http::isSuccessStatus(Http::Status::PermanentRedirect));

        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::BadRequest));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::Unauthorized));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::PaymentRequired));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::Forbidden));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::NotFound));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::MethodNotAllowed));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::NotAcceptable));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::ProxyAuthenticationRequired));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::RequestTimeout));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::Conflict));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::Gone));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::LengthRequired));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::PreconditionFailed));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::PayloadTooLarge));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::UriTooLong));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::UnsupportedMediaType));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::RangeNotSatisfiable));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::ExpectationFailed));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::ImATeapot));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::MisdirectedRequest));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::UnprocessableEntity));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::Locked));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::FailedDependency));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::UpgradeRequired));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::PreconditionRequired));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::TooManyRequests));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::RequestHeaderFieldsTooLarge));
        EXPECT_FALSE(Http::isRedirectionStatus(Http::Status::UnavailableForLegalReasons));

        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::InternalServerError));
        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::NotImplemented));
        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::BadGateway));
        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::ServiceUnavailable));
        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::GatewayTimeout));
        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::HttpVersionNotSupported));
        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::VariantAlsoNegotiates));
        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::InsufficientStorage));
        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::LoopDetected));
        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::NotExtended));
        EXPECT_FALSE(Http::isClientErrorStatus(Http::Status::NetworkAuthenticationRequired));
    }
}