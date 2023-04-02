#ifndef CONNECTIVITY_HTTP_H
#define CONNECTIVITY_HTTP_H

#include <ESPAsyncWebServer.h>
#include <string>

namespace Connectivity
{
    namespace HTTP
    {
        struct Header
        {
            Header(const std::string& key, const std::string& value) : key(key), value(value)
            {}
            std::string key;
            std::string value;
        };

        enum class Method : uint8_t
        {
            Get = HTTP_GET,
            Post = HTTP_POST,
            Delete = HTTP_DELETE,
            Put = HTTP_PUT,
            Patch = HTTP_PATCH,
            Head = HTTP_HEAD,
            Options = HTTP_OPTIONS
        };

        enum class StatusCode : uint16_t
        {
            Continue = 100,
            SwitchingProtocols = 101,
            Processing = 102,
            EarlyHints = 103,

            OK = 200,
            Created = 201,
            Accepted = 202,
            NonAuthoritativeInformatinon = 203,
            NoContent = 204,
            ResetContent = 205,
            PartialContent = 206,
            MultiStatus = 207,
            AlreadyReported = 208,
            ImUsed = 226,

            MultipleChoices = 300,
            MovedPermanently = 301,
            Found = 302,
            SeeOther = 303,
            NotModified = 304,
            UseProxy = 305,
            SwitchProxy = 306,
            TemporaryRedirect = 307,
            PermanentRedirect = 308,

            BadRequest = 400,
            Unauthorized = 401,
            PaymentRequired = 402,
            Forbidden = 403,
            NotFound = 404,
            MethodNotAllowed = 405,
            NotAcceptable = 406,
            ProxyAuthenticationRequired = 407,
            RequestTimeout = 408,
            Conflict = 409,
            Gone = 410,
            LengthRequired = 411,
            PreconditionFailed = 412,
            RequestEntityTooLarge = 413,
            RequestUriTooLong = 414,
            UnsupportedMediaType = 415,
            RequestedRangeNotSatisfiable = 416,
            ExpectationFailed = 417,
            IAmATeapot = 418,
            UnprocessableEntity = 422,
            Locked = 423,
            FailedDependency = 424,
            UpgradeRequired = 426,
            PreconditionRequired = 428,

            InternalServerError = 500,
            NotImplemented = 501,
            BadGateway = 502,
            ServiceUnavailable = 503,
            GatewayTimeout = 504,
            HttpVersionNotSupported = 505,
            VariantAlsoNegotiates = 506,
            InsufficientStorage = 507,
            LoopDetected = 508,
            BandwidthLimitExceeded = 509,
            NotExtended = 510,
            NetworkAuthenticationRequired = 511
        };
    }
}

#endif