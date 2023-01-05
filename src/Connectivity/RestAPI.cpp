#ifdef ESP32

#include "Connectivity/RestAPI.h"
#include "Logging/Log.h"
#include "Error/ExceptionStack.h"

using namespace Connectivity;


RestAPI::RestAPI(AsyncWebServer* server, const std::string& baseURI, bool allowCORS) : 
    m_server(server),
    m_baseURI(baseURI),
    m_allowCORS(allowCORS)
{
    if(allowCORS)
    {
        m_server->on(m_baseURI.c_str(), HTTP_OPTIONS, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(204);
            addCORSHeaders(response);
            request->send(response);
        });
    }
}


void RestAPI::handle(WebRequestMethod method, const std::string& endpointURI, const JsonHandler& handler)
{
    switch(method)
    {
    case HTTP_GET:
        handleWithoutBody(method, endpointURI, handler);
        handleHead(endpointURI);
        break;
    
    case HTTP_DELETE:
        handleWithoutBody(method, endpointURI, handler);
        break;

    default:
        handleWithBody(method, endpointURI, handler);
        break;
    }
}


void RestAPI::handleWithoutBody(WebRequestMethod method, const std::string& endpointURI, const JsonHandler& handler)
{
    m_server->on((m_baseURI + endpointURI).c_str(), method, [handler, this](AsyncWebServerRequest* request){
        json data;
        uint16_t statusCode = 500;

        try
        {
            std::tie(data, statusCode) = handler(json());
        }
        catch(const std::exception& e)
        {
            // data["error"] = Error::ExceptionStack::what(e);
            Logging::Logger[Level::Error] << SOURCE_LOCATION << data["error"] << std::endl;
        }

        AsyncWebServerResponse* response = request->beginResponse(statusCode, "application/json", data.dump().c_str());
        addCORSHeaders(response);
        request->send(response);
    });
}


void RestAPI::handleWithBody(WebRequestMethod method, const std::string& endpointURI, const JsonHandler& handler)
{
    m_server->on((m_baseURI + endpointURI).c_str(), method, 
        [](AsyncWebServerRequest*){},
        [](AsyncWebServerRequest*, const String&, size_t, uint8_t*, size_t, bool){},
        [handler, this](AsyncWebServerRequest* request, uint8_t* rawData, size_t length, size_t, size_t) {
            json data;
            uint16_t statusCode = 500;

            try
            {
                std::string stringData = reinterpret_cast<char*>(rawData);
                stringData.resize(length);
                std::tie(data, statusCode) = handler(json::parse(stringData));
            }
            catch(const std::exception& e)
            {
                // data["error"] = Error::ExceptionStack::what(e);
                Logging::Logger[Level::Error] << SOURCE_LOCATION << data["error"] << std::endl;
            }

            AsyncWebServerResponse* response = request->beginResponse(statusCode, "application/json", data.dump().c_str());
            addCORSHeaders(response);
            request->send(response);
        }
    );
}


void RestAPI::handleHead(const std::string& endpointURI)
{
    m_server->on((m_baseURI + endpointURI).c_str(), HTTP_HEAD, [this](AsyncWebServerRequest* request){
        AsyncWebServerResponse* response = request->beginResponse(204);
        addCORSHeaders(response);
        request->send(response);
    });
}


void RestAPI::addCORSHeaders(AsyncWebServerResponse* response)
{
    if(m_allowCORS)
    {
        response->addHeader("Access-Control-Request-Method", "*");
        response->addHeader("Access-Control-Expose-Headers", "*"); 
        response->addHeader("Access-Control-Allow-Methods", "*");
        response->addHeader("Access-Control-Allow-Origin", "*");
        response->addHeader("Access-Control-Allow-Headers", "*");
    }
}

#endif