#ifdef ESP32

#include "Connectivity/RestAPI.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"


using namespace Connectivity;


RestAPI::RestAPI(AsyncWebServer* server, const std::string& baseURI, bool allowCORS) noexcept : 
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


void RestAPI::handle(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler) noexcept
{
    switch(method)
    {
    case HTTP::Method::Get:
        handleWithoutBody(method, endpointURI, handler);
        handleHead(endpointURI);
        break;
    
    case HTTP::Method::Delete:
        handleWithoutBody(method, endpointURI, handler);
        break;

    case HTTP::Method::Head:
        handleHead(endpointURI);
        break;

    default:
        handleWithBody(method, endpointURI, handler);
        break;
    }
}


void RestAPI::handleWithoutBody(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler) noexcept
{
    m_server->on((m_baseURI + endpointURI).c_str(), static_cast<WebRequestMethod>(method), [handler, this](AsyncWebServerRequest* request){
        JsonResponse jsonResponse;
        try
        {
            jsonResponse = handler(json());
        }
        catch(...)
        {
            jsonResponse.data["error"] = Diagnostics::ExceptionTrace::what();
            Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
                << jsonResponse.data["error"].get<std::string>() << std::endl;
        }

        AsyncWebServerResponse* response = request->beginResponse(
            static_cast<int>(jsonResponse.status),
            "application/json",
            jsonResponse.data.dump(1, '\t').c_str()
        );
        
        addCORSHeaders(response);
        request->send(response);
    });
}


void RestAPI::handleWithBody(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler) noexcept
{
    m_server->on((m_baseURI + endpointURI).c_str(), static_cast<WebRequestMethod>(method), 
        [](AsyncWebServerRequest*){},
        [](AsyncWebServerRequest*, const String&, size_t, uint8_t*, size_t, bool){},
        [handler, this](AsyncWebServerRequest* request, uint8_t* rawData, size_t length, size_t, size_t) {
            JsonResponse jsonResponse;
            jsonResponse.status = HTTP::StatusCode::InternalServerError;
            try
            {
                std::string stringData = reinterpret_cast<char*>(rawData);
                stringData.resize(length);
                jsonResponse = handler(json::parse(stringData));
            }
            catch(...)
            {
                jsonResponse.data["error"] = Diagnostics::ExceptionTrace::what();
                Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
                    << jsonResponse.data["error"].get<std::string>() << std::endl;
            }

            AsyncWebServerResponse* response = request->beginResponse(
                static_cast<int>(jsonResponse.status), 
                "application/json",
                jsonResponse.data.dump(1, '\t').c_str()
            );

            addCORSHeaders(response);
            request->send(response);
        }
    );
}


void RestAPI::handleHead(const std::string& endpointURI) noexcept
{
    m_server->on((m_baseURI + endpointURI).c_str(), HTTP_HEAD, [this](AsyncWebServerRequest* request){
        AsyncWebServerResponse* response = request->beginResponse(204);
        addCORSHeaders(response);
        request->send(response);
    });
}


void RestAPI::addCORSHeaders(AsyncWebServerResponse* response) noexcept
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