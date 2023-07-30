#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <unordered_map>


namespace HTTP
{
    using Header = std::pair<std::string, std::string>;
    using HeaderMap = std::unordered_map<std::string, std::string>;

    struct Method
    {
        enum Value : uint16_t
        {
            GET = 1,
            POST = 3,
            DELETE = 0,
            PUT = 4,
            PATCH = 28,
            HEAD = 2,
            OPTIONS = 6
        };

        constexpr Method(Value value) : value(value) {}
        constexpr operator Value() const { return value; }
        explicit operator bool() const = delete;

        operator std::string() const;
    
        Value value;
    };
            
    struct StatusCode
    {
        enum Value : uint16_t
        {
            Continue = 100,
            SwitchingProtocols = 101,
            Processing = 102,
            EarlyHints = 103,

            OK = 200,
            Created = 201,
            Accepted = 202,
            NonAuthoritativeInformation = 203,
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
            PayloadTooLarge = 413,
            UriTooLong = 414,
            UnsupportedMediaType = 415,
            RangeNotSatisfiable = 416,
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

        constexpr StatusCode(Value value) : value(value) {}
        constexpr operator Value() const { return value; }
        explicit operator bool() const = delete;

        std::string getDescription() const;

        Value value;
    };
}

#endif