#ifndef CONNECTIVITY_RESTAPI_H
#define CONNECTIVITY_RESTAPI_H

#include "Connectivity/HTTP.h"

#include <json.hpp>
#include <functional>
#include <string>
#include <utility>

namespace Connectivity
{
    class RestAPI
    {
    public:
        struct JsonResponse
        {
            JsonResponse(const json& data = nullptr, HTTP::StatusCode status = HTTP::StatusCode::OK) :
                data(data),
                status(status)
            {}
            json data;
            HTTP::StatusCode status;
        };

        using JsonHandler = std::function<JsonResponse(const json&)>;

        RestAPI(AsyncWebServer* server, const std::string& baseURI = "/", bool allowCORS = true) noexcept;
        void handle(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler) noexcept;

    private:
        void handleWithoutBody(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler) noexcept;
        void handleWithBody(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler) noexcept;
        void handleHead(const std::string& endpointURI) noexcept;
        void addCORSHeaders(AsyncWebServerResponse* response) noexcept;

        AsyncWebServer* m_server;
        std::string m_baseURI;
        bool m_allowCORS;
    };
}

#endif
