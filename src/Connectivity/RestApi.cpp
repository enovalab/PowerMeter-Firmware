#include "RestAPI.h"

using namespace Connectivity;


RestAPI::RestAPI(AsyncWebServer& server, const std::string& baseURI, bool allowCORS) : 
    m_server(server),
    m_baseURI(baseURI),
    m_allowCORS(allowCORS)
{
    if(m_allowCORS)
    {
        server.on(m_baseURI.c_str(), HTTP_OPTIONS, [](AsyncWebServerRequest* request) {
            request->send(204);
        });
    }
    m_server.begin();
}

void RestAPI::handleGet(const std::string& endpointURI, const JsonGetter& getData)
{
    m_server.on((m_baseURI + endpointURI).c_str(), HTTP_GET, [getData, this](AsyncWebServerRequest* request){
        AsyncWebServerResponse* response = request->beginResponse(200, "application/json", getData().dump().c_str());
        addCORSHeaders(response);
        request->send(response);
    });

    m_server.on((m_baseURI + endpointURI).c_str(), HTTP_HEAD, [this](AsyncWebServerRequest* request){
        AsyncWebServerResponse* response = request->beginResponse(200, "application/json");
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