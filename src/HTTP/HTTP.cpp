#include "HTTP.h"

using namespace HTTP;

Method::operator std::string() const
{
    switch(value)
    {
        case GET:       return "GET";
        case POST:      return "POST";
        case DELETE:    return "DELETE";
        case PUT:       return "PUT";
        case PATCH:     return "PATCH";
        case HEAD:      return "HEAD";
        case OPTIONS:   return "OPTIONS";
        default:        return "";
    }
}


std::string StatusCode::getDescription() const
{
    switch(value)
    {
        case Continue:                      return "Continue";
        case SwitchingProtocols:            return "Switching Protocols";
        case Processing:                    return "Processing";
        case EarlyHints:                    return "Early Hints";
        case OK:                            return "OK";
        case Created:                       return "Created";
        case Accepted:                      return "Accepted";
        case NonAuthoritativeInformation:   return "Non-Authoritative Informatinon";
        case NoContent:                     return "No Content";
        case ResetContent:                  return "Reset Content";
        case PartialContent:                return "Partial Content";
        case MultiStatus:                   return "Multi Status";
        case AlreadyReported:               return "Already Reported";
        case ImUsed:                        return "IM Used";
        case MultipleChoices:               return "Multiple Choices";
        case MovedPermanently:              return "Moved Permanently";
        case Found:                         return "Found";
        case SeeOther:                      return "See Other";
        case NotModified:                   return "Not Modified";
        case UseProxy:                      return "Use Proxy";
        case SwitchProxy:                   return "Switch Proxy";
        case TemporaryRedirect:             return "Temporary Redirect";
        case PermanentRedirect:             return "Permanent Redirect";
        case BadRequest:                    return "Bad Request";
        case Unauthorized:                  return "Unauthorized";
        case PaymentRequired:               return "Payment Required";
        case Forbidden:                     return "Forbidden";
        case NotFound:                      return "Not Found";
        case MethodNotAllowed:              return "Method Not Allowed";
        case NotAcceptable:                 return "Not Acceptable";
        case ProxyAuthenticationRequired:   return "Proxy Authentication Required";
        case RequestTimeout:                return "Request Timeout";
        case Conflict:                      return "Conflict";
        case Gone:                          return "Gone";
        case LengthRequired:                return "Length Required";
        case PreconditionFailed:            return "Precondition Failed";
        case PayloadTooLarge:               return "Payload Too Large";
        case UriTooLong:                    return "URI Too Long";
        case UnsupportedMediaType:          return "Unsupported Media Type";
        case RangeNotSatisfiable:           return "Range Not Satisfiable";
        case ExpectationFailed:             return "Expectation Failed";
        case IAmATeapot:                    return "I'm a teapot";
        case UnprocessableEntity:           return "Unprocessable Entity";
        case Locked:                        return "Locked";
        case FailedDependency:              return "Failed Dependency";
        case UpgradeRequired:               return "Upgrade Required";
        case PreconditionRequired:          return "Precondition Required";
        case InternalServerError:           return "Internal Server Error";
        case NotImplemented:                return "Not Implemented";
        case BadGateway:                    return "Bad Gateway";
        case ServiceUnavailable:            return "Service Unavailable";
        case GatewayTimeout:                return "Gateway Timeout";
        case HttpVersionNotSupported:       return "HTTP Version Not Supported";
        case VariantAlsoNegotiates:         return "Variant Also Negotiates";
        case InsufficientStorage:           return "Insufficient Storage";
        case LoopDetected:                  return "Loop Detected";
        case BandwidthLimitExceeded:        return "Bandwidth Limit Exceeded";
        case NotExtended:                   return "Not Extended";
        case NetworkAuthenticationRequired: return "Network Authentication Required";
        default: return "";
    }
}