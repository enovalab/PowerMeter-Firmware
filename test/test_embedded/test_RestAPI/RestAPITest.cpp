#include "Connectivity/RestAPI.h"
#include "Logging/Log.h"

#include <Arduino.h>

AsyncWebServer server(80);
Connectivity::RestAPI api(&server);


void setup()
{
    Serial.begin(115200);
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Power Meter", "Katek1976");

    api.handle(HTTP_GET, "", [](json){
        Logging::Logger[Level::Info] << "GET handled" << std::endl;
        return Connectivity::RestAPI::JsonResponse();
    });

    api.handle(HTTP_DELETE, "", [](json){
        Logging::Logger[Level::Info] << "DELETE handled" << std::endl;
        return Connectivity::RestAPI::JsonResponse();
    });

    api.handle(HTTP_PUT, "", [](json data){
        Logging::Logger[Level::Info] << "PUT handled" << std::endl;
        Logging::Logger[Level::Info] << data.dump(2, ' ') << '\n' << std::endl;
        return Connectivity::RestAPI::JsonResponse();
    });

    api.handle(HTTP_PATCH, "", [](json data){
        Logging::Logger[Level::Info] << "PATCH handled" << std::endl;
        Logging::Logger[Level::Info] << data.dump(2, ' ') << '\n' << std::endl;
        return Connectivity::RestAPI::JsonResponse();
    });

    api.handle(HTTP_POST, "", [](json data){
        Logging::Logger[Level::Info] << "POST handled" << std::endl;
        Logging::Logger[Level::Info] << data.dump(2, ' ') << '\n' << std::endl;
        return Connectivity::RestAPI::JsonResponse();
    });

    server.begin();
}

void loop()
{}