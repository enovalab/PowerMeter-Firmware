#include "Connectivity/RestAPI.h"
#include "Logging/Log.h"

#include <Arduino.h>

AsyncWebServer server(80);
Connectivity::RestAPI api(&server);


void setup()
{
    Serial.begin(115200);
    
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("RestAPI Test", "Katek1976");
    Logging::Logger[Level::Info] << "AP IP Address is: " << WiFi.softAPIP().toString().c_str() << std::endl;

    WiFi.begin("WLAN-T-Com", "GxvuzR8znV");
    while(WiFi.status() != WL_CONNECTED);
    Logging::Logger[Level::Info] << "STA IP Address is: " << WiFi.localIP().toString().c_str() << std::endl;

    api.handle(HTTP_GET, "", [](json){
        Logging::Logger[Level::Info] << "GET handled" << std::endl;
        json data = R"( {"message": "Hello from RestAPI"} )"_json;
        return Connectivity::RestAPI::JsonResponse(data);
    });

    api.handle(HTTP_DELETE, "", [](json){
        Logging::Logger[Level::Info] << "DELETE handled" << std::endl;
        return Connectivity::RestAPI::JsonResponse();
    });

    api.handle(HTTP_PUT, "", [](json data){
        Logging::Logger[Level::Info] << "PUT handled" << std::endl;
        Logging::Logger[Level::Info] << data.dump(2, ' ') << '\n' << std::endl;
        return Connectivity::RestAPI::JsonResponse(data);
    });

    api.handle(HTTP_PATCH, "", [](json data){
        Logging::Logger[Level::Info] << "PATCH handled" << std::endl;
        Logging::Logger[Level::Info] << data.dump(2, ' ') << '\n' << std::endl;
        return Connectivity::RestAPI::JsonResponse(data);
    });

    api.handle(HTTP_POST, "", [](json data){
        Logging::Logger[Level::Info] << "POST handled" << std::endl;
        Logging::Logger[Level::Info] << data.dump(2, ' ') << '\n' << std::endl;
        return Connectivity::RestAPI::JsonResponse(data);
    });

    server.begin();
}

void loop()
{}