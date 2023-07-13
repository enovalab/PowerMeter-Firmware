#ifdef ESP32

#include "Connectivity/RestAPI.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"


using namespace Connectivity;


RestAPI::RestAPI(AsyncWebServer* server, const std::string& baseURI, const HeaderList& defaultHeaders) noexcept : 
    m_server(server),
    m_baseURI(baseURI),
    m_defaultHeaders(defaultHeaders)
{
    m_server->on(m_baseURI.c_str(), HTTP_OPTIONS, [this](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(static_cast<uint16_t>(HTTP::StatusCode::NoContent));
        addHeaders(response, {});
        request->send(response);
    });
}


void RestAPI::handle(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler, const HeaderList& headers) noexcept
{
    switch(method)
    {
    case HTTP::Method::Get:
        handleWithoutBody(method, endpointURI, handler, headers);
        handleHead(endpointURI, headers);
        break;
    
    case HTTP::Method::Delete:
        handleWithoutBody(method, endpointURI, handler, headers);
        break;

    case HTTP::Method::Head:
        handleHead(endpointURI, headers);
        break;

    default:
        handleWithBody(method, endpointURI, handler, headers);
        break;
    }
}


void RestAPI::handleWithoutBody(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler, const HeaderList& headers) noexcept
{
    m_server->on((m_baseURI + endpointURI).c_str(), static_cast<WebRequestMethod>(method), [handler, headers, this](AsyncWebServerRequest* request){
        JsonResponse jsonResponse;
        try
        {
            jsonResponse = handler(json());
        }
        catch(...)
        {
            jsonResponse.data = Diagnostics::ExceptionTrace::what();
            Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
                << jsonResponse.data.get<std::string>() << std::endl;
        }

        AsyncWebServerResponse* response = request->beginResponse(
            static_cast<uint16_t>(jsonResponse.status),
            "application/json",
            jsonResponse.data.dump(1, '\t').c_str()
        );
        
        addHeaders(response, headers);
        request->send(response);
    });
}


void RestAPI::handleWithBody(HTTP::Method method, const std::string& endpointURI, const JsonHandler& handler, const HeaderList& headers) noexcept
{
    m_server->on((m_baseURI + endpointURI).c_str(), static_cast<WebRequestMethod>(method), 
        [](AsyncWebServerRequest*){},
        [handler, headers, this](AsyncWebServerRequest* request, const String& filename, size_t index, uint8_t* rawData, size_t length, bool final){
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
                jsonResponse.data = Diagnostics::ExceptionTrace::what();
                Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
                    << jsonResponse.data.get<std::string>() << std::endl;
            }

            AsyncWebServerResponse* response = request->beginResponse(
                static_cast<uint16_t>(jsonResponse.status), 
                "application/json",
                jsonResponse.data.dump(1, '\t').c_str()
            );

            addHeaders(response, headers);
            request->send(response);
        },
        [handler, headers, this](AsyncWebServerRequest* request, uint8_t* rawData, size_t length, size_t, size_t) {
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
                jsonResponse.data = Diagnostics::ExceptionTrace::what();
                Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
                    << jsonResponse.data.get<std::string>() << std::endl;
            }

            AsyncWebServerResponse* response = request->beginResponse(
                static_cast<uint16_t>(jsonResponse.status), 
                "application/json",
                jsonResponse.data.dump(1, '\t').c_str()
            );

            addHeaders(response, headers);
            request->send(response);
        }
    );
}


void RestAPI::handleHead(const std::string& endpointURI, const HeaderList& headers) noexcept
{
    m_server->on((m_baseURI + endpointURI).c_str(), HTTP_HEAD, [headers, this](AsyncWebServerRequest* request){
        AsyncWebServerResponse* response = request->beginResponse(204);
        addHeaders(response, headers);
        request->send(response);
    });
}


void RestAPI::addHeaders(AsyncWebServerResponse* response, const HeaderList& headers) noexcept
{
    for(const auto& header : m_defaultHeaders)
    {
        response->addHeader(header.key.c_str(), header.value.c_str());
    }

    for(const auto& header : headers)
    {
        response->addHeader(header.key.c_str(), header.value.c_str());
    }
}

#endif