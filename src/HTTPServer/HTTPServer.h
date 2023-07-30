#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "HTTP/HTTP.h"
#include <functional>
#include <esp_http_server.h>


class HTTPServer
{
public:
    struct Request
    {
        Request(const std::string& uri, HTTP::Method method, const std::string& body) :
            uri(uri),
            method(method),
            body(body)
        {}
        std::string uri;
        HTTP::Method method;
        std::string body;
    };

    struct Response
    {
        Response(
            const std::string& body,
            const std::string& contentType = "text/plain",
            HTTP::StatusCode status = HTTP::StatusCode::OK,
            const HTTP::HeaderMap& headers = {}
        ) : 
            body(body),
            contentType(contentType),
            status(status),
            headers(headers)
        {}
        HTTP::StatusCode status;
        std::string contentType;
        std::string body;
        HTTP::HeaderMap headers;
    };

    using RequestHandler = std::function<Response(const Request&)>;

    HTTPServer(uint16_t port = 80, const HTTP::HeaderMap& defaultHeaders = {});
    ~HTTPServer();
    virtual void start();
    virtual void stop();
    void registerURI(const std::string& uri, HTTP::Method method, const RequestHandler& handlerCallback) noexcept;
    void unregisterURI(const std::string& uri, HTTP::Method method) noexcept;
    void unregisterURI(const std::string& uri) noexcept;

protected:
    uint16_t m_port;
    httpd_handle_t m_server;
    HTTP::HeaderMap m_defaultHeaders;
};

#endif