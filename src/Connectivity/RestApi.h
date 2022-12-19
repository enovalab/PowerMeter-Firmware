#ifndef CONNECTIVITY_RESTAPI_H
#define CONNECTIVITY_RESTAPI_H

#include <ESPAsyncWebServer.h>
#include <json.hpp>
#include <functional>
#include <string>

namespace Connectivity
{
    using JsonGetter = std::function<json()>;
    using JsonSetter = std::function<void(json)>;

    class RestAPI
    {
    public:
        RestAPI(AsyncWebServer& server, const std::string& baseURI = "/", bool allowCORS = true);
        void handleGet(const std::string& endpointURI, const JsonGetter& getData);
        void handlePut(const std::string& endpointURI, const JsonSetter& setData);
        void handlePatch(const std::string& endpointURI, const JsonSetter& setData);
        void handlePost(const std::string& endpointURI, const JsonSetter& setData);
        void handleDelete(const std::string& endpointURI, const std::function<void()>& deleteData);

    private:
        void addCORSHeaders(AsyncWebServerResponse* response);

        AsyncWebServer m_server;
        std::string m_baseURI;
        bool m_allowCORS;
    };
}

#endif