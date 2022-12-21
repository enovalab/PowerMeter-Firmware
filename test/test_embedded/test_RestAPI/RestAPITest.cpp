#include "Connectivity/RestAPI.h"
#include "Logging/Log.h"

#include <Arduino.h>

AsyncWebServer server(80);
Connectivity::RestAPI api(&server, "/", true);


void setup()
{
    Serial.begin(115200);
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Power Meter", "Katek1976");

    Logging::Logger[Level::Info] << "started" << std::endl;

    api.handle(HTTP_GET, "foo", [](json data){
        Logging::Logger[Level::Info] << "handled" << std::endl;
        return Connectivity::RestAPI::JsonResponse();
    });

    server.begin();
}

void loop()
{
}