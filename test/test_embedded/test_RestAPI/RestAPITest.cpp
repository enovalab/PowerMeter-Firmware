#include "Connectivity/RestAPI.h"
#include "Diagnostics/Log.h"

AsyncWebServer server(80);

void setup()
{
    Serial.begin(115200);
    
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("RestAPI Test", "Katek1976");
    Diagnostics::Logger[Level::Info] << "AP IP Address is: " << WiFi.softAPIP().toString().c_str() << std::endl;

    Connectivity::RestAPI api(&server);

    api.handle(Connectivity::HTTP::Method::Get, "", [](json){
        Diagnostics::Logger[Level::Info] << "GET handled" << std::endl;
        json data = R"( {"message": "Hello from RestAPI"} )"_json;
        return Connectivity::RestAPI::JsonResponse(data);
    });

    api.handle(Connectivity::HTTP::Method::Delete, "", [](json){
        Diagnostics::Logger[Level::Info] << "DELETE handled" << std::endl;
        return Connectivity::RestAPI::JsonResponse();
    });

    api.handle(Connectivity::HTTP::Method::Put, "", [](const json& data){
        Diagnostics::Logger[Level::Info] << "PUT handled" << std::endl;
        Diagnostics::Logger[Level::Info] << "\n" << data.dump(2, ' ') << '\n' << std::endl;
        return Connectivity::RestAPI::JsonResponse(data);
    });

    api.handle(Connectivity::HTTP::Method::Patch, "", [](const json& data){
        Diagnostics::Logger[Level::Info] << "PATCH handled" << std::endl;
        Diagnostics::Logger[Level::Info] << "\n" << data.dump(2, ' ') << '\n' << std::endl;
        return Connectivity::RestAPI::JsonResponse(data);
    });

    api.handle(Connectivity::HTTP::Method::Post, "", [](const json& data){
        Diagnostics::Logger[Level::Info] << "POST handled" << std::endl;
        Diagnostics::Logger[Level::Info] << "\n" << data.dump(2, ' ') << '\n' << std::endl;
        return Connectivity::RestAPI::JsonResponse(data);
    });

    server.begin();
}

void loop()
{}