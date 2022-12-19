#include "Connectivity/RestAPI.h"

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


void RestAPI::handle(const std::string& endpointURI, WebRequestMethod method, const JsonHandler& handler)
{

}


void RestAPI::handleGet(const std::string& endpointURI, const JsonHandler& handler)
{
    m_server.on((m_baseURI + endpointURI).c_str(), HTTP_GET, [handler, this](AsyncWebServerRequest* request){
        JsonResponse jsonResponse = handler(json());
        AsyncWebServerResponse* response;

        if(jsonResponse.statusCode >= 200 && jsonResponse.statusCode < 300)
            response = request->beginResponse(jsonResponse.statusCode, "application/json", jsonResponse.data.dump().c_str());
        else
            response = request->beginResponse(jsonResponse.statusCode, "text/plain", jsonResponse.errorMessage.c_str());

        addCORSHeaders(response);
        request->send(response);
    });

    m_server.on((m_baseURI + endpointURI).c_str(), HTTP_HEAD, [handler, this](AsyncWebServerRequest* request){
        JsonResponse jsonResponse = handler(json());
        AsyncWebServerResponse* response;

        if(jsonResponse.statusCode >= 200 && jsonResponse.statusCode < 300)
            response = request->beginResponse(jsonResponse.statusCode, "application/json");
        else
            response = request->beginResponse(jsonResponse.statusCode, "text/plain");

        addCORSHeaders(response);
        request->send(response);
    });
}


void RestAPI::handlePut(const std::string& endpointURI, const JsonHandler& handler)
{
    m_server.on((m_baseURI + endpointURI).c_str(), HTTP_PUT, 
        [](AsyncWebServerRequest*){},
        [](AsyncWebServerRequest*, const String&, size_t, uint8_t*, size_t, bool){},
        [handler, this](AsyncWebServerRequest* request, uint8_t* rawData, size_t length, size_t index, size_t total) {
            JsonResponse jsonResponse = handler(json::parse(reinterpret_cast<char*>(rawData)));
            AsyncWebServerResponse* response;

            if(jsonResponse.statusCode >= 200 && jsonResponse.statusCode < 300)
                response = request->beginResponse(jsonResponse.statusCode, "application/json", jsonResponse.data.dump().c_str());
            else
                response = request->beginResponse(jsonResponse.statusCode, "text/plain", jsonResponse.errorMessage.c_str());

            addCORSHeaders(response);
            request->send(response);
        }
    );
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