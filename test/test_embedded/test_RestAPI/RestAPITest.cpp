#include "Connectivity/RestAPI.h"
#include "Diagnostics/Log.h"
#include <Arduino.h>
#include <WiFi.h>

using namespace Connectivity;

HTTPServer server;

void setup()
{
    Serial.begin(115200);
    WiFi.begin("WLAN-T-Com", "GxvuzR8znV");
    delay(3000);
    Diagnostics::Logger[Level::Info] << "IP Address is: " << WiFi.localIP().toString().c_str() << std::endl;

    server.start();
    RestAPI api(server);

    api.registerURI("/", HTTP::Method::Get, [](json){
        Diagnostics::Logger[Level::Info] << "GET handled" << std::endl;
        json data = "Hello from RestAPI";
        return RestAPI::JsonResponse(data);
    });

    api.registerURI("/", HTTP::Method::Delete, [](json){
        Diagnostics::Logger[Level::Info] << "DELETE handled" << std::endl;
        return RestAPI::JsonResponse();
    });

    api.registerURI("/", HTTP::Method::Put, [](const json& data){
        Diagnostics::Logger[Level::Info] << "PUT handled" << std::endl;
        Diagnostics::Logger[Level::Info] << "\n" << data.dump(2, ' ') << '\n' << std::endl;
        return RestAPI::JsonResponse(data);
    });

    api.registerURI("/", HTTP::Method::Patch, [](const json& data){
        Diagnostics::Logger[Level::Info] << "PATCH handled" << std::endl;
        Diagnostics::Logger[Level::Info] << "\n" << data.dump(2, ' ') << '\n' << std::endl;
        return RestAPI::JsonResponse(data);
    });

    api.registerURI("/", HTTP::Method::Post, [](const json& data){
        Diagnostics::Logger[Level::Info] << "POST handled" << std::endl;
        Diagnostics::Logger[Level::Info] << "\n" << data.dump(2, ' ') << '\n' << std::endl;
        return RestAPI::JsonResponse(data);
    });
}

void loop()
{}