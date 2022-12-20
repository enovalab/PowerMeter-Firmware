#include <Arduino.h>
#include "Connectivity/RestAPI.h"
#include "Logging/Log.h"

AsyncWebServer server(80);
Connectivity::RestAPI api(&server, "/", true);


void setup()
{
    Serial.begin(115200);
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Power Meter", "Katek1976");

    Logging::Logger[Logging::Level::Info] << "started" << std::endl;

    api.handle(HTTP_GET, "foo", [](json data){
        Logging::Logger[Logging::Level::Info] << "handled" << std::endl;
        return Connectivity::RestAPI::JsonResponse();
    });

    server.begin();
}

void loop()
{
}