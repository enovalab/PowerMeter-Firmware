#include "Connectivity/RestAPI.h"
#include "Logging/Log.h"

using namespace Connectivity;


RestAPI::RestAPI(AsyncWebServer* server, const std::string& baseURI, bool allowCORS) : 
    m_server(server),
    m_baseURI(baseURI),
    m_allowCORS(allowCORS)
{
    if(m_allowCORS)
    {
        m_server->on(m_baseURI.c_str(), HTTP_OPTIONS, [](AsyncWebServerRequest* request) {
            request->send(204);
        });
    }
    
}


void RestAPI::handle(WebRequestMethod method, const std::string& endpointURI, const JsonHandler& handler)
{
    switch(method)
    {
    case HTTP_GET:
        Logging::Logger[Logging::Level::Info] << "GET" << std::endl;
        Logging::Logger[Logging::Level::Info] << (m_baseURI + endpointURI).c_str() << std::endl;
        m_server->on((m_baseURI + endpointURI).c_str(), HTTP_GET, [handler, this](AsyncWebServerRequest* request){
            JsonResponse jsonResponse = handler(json());
            AsyncWebServerResponse* response;

            if(jsonResponse.statusCode >= 200 && jsonResponse.statusCode < 300)
                response = request->beginResponse(jsonResponse.statusCode, "application/json", jsonResponse.data.dump().c_str());
            else
                response = request->beginResponse(jsonResponse.statusCode, "text/plain", jsonResponse.errorMessage.c_str());

            addCORSHeaders(response);
            request->send(response);

            request->send(200);
        });

        m_server->on((m_baseURI + endpointURI).c_str(), HTTP_HEAD, [handler, this](AsyncWebServerRequest* request){
            JsonResponse jsonResponse = handler(json());
            AsyncWebServerResponse* response;

            if(jsonResponse.statusCode >= 200 && jsonResponse.statusCode < 300)
                response = request->beginResponse(jsonResponse.statusCode, "application/json");
            else
                response = request->beginResponse(jsonResponse.statusCode, "text/plain", jsonResponse.errorMessage.c_str());

            addCORSHeaders(response);
            request->send(response);
        });
        break;
    
    case HTTP_DELETE:
        Logging::Logger[Logging::Level::Info] << "DELETE"<< std::endl;
        m_server->on((m_baseURI + endpointURI).c_str(), HTTP_DELETE, [handler, this](AsyncWebServerRequest* request){
            JsonResponse jsonResponse = handler(json());
            AsyncWebServerResponse* response;

            if(jsonResponse.statusCode >= 200 && jsonResponse.statusCode < 300)
                response = request->beginResponse(jsonResponse.statusCode, "application/json");
            else
                response = request->beginResponse(jsonResponse.statusCode, "text/plain", jsonResponse.errorMessage.c_str());

            addCORSHeaders(response);
            request->send(response);
        });
        break;

    default:
        Logging::Logger[Logging::Level::Info] << "Other"<< std::endl;
        m_server->on((m_baseURI + endpointURI).c_str(), method, 
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
        });
        break;
    }
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
