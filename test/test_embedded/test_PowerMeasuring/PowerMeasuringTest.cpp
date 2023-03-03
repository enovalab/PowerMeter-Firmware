#include "Measuring/ACPowerMeter.h"
#include "Connectivity/RestAPI.h"
#include "Diagnostics/Log.h"

#include <AsyncElegantOTA.h>

constexpr uint8_t voltagePin = 33;
constexpr uint8_t currentPin = 32;

constexpr float voltageCalibration = 587.1;
constexpr float currentCalibration = 15.7;
constexpr float phaseCalibration = -1.1;


Measuring::ACPowerMeter powerMeter(voltagePin, currentPin);
AsyncWebServer server(80);
Measuring::ACPower power(0, 0, 0);


void setup()
{
    Serial.begin(115200);

    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("Power Measuring Test", "Katek1976");
    Diagnostics::Logger[Level::Info] << "AP IP Address is: " << WiFi.softAPIP().toString().c_str() << std::endl;

    powerMeter.calibrate(voltageCalibration, currentCalibration, phaseCalibration);
    
    Connectivity::RestAPI api(&server, "/api");
    
    api.handle(Connectivity::HTTP::Method::Get, "/power", [](json){
        json data;
        power = powerMeter.measure();
        data["voltage"] = power.getVoltageRms();
        data["current"] = power.getCurrentRms();
        data["active"] = power.getActivePower();
        data["apparent"] = power.getApparentPower();
        data["reactive"] = power.getReactivePower();
        data["powerFactor"] = power.getPowerFactor();
        return Connectivity::RestAPI::JsonResponse(data);
    });

    api.handle(Connectivity::HTTP::Method::Get, "/toggle", [](json){
        digitalWrite(2, !digitalRead(2));
        return Connectivity::RestAPI::JsonResponse();
    });

    AsyncElegantOTA.begin(&server);
    server.begin();
}


void loop()
{
    power = powerMeter.measure();
    Diagnostics::Logger[Level::Info]
        << "U = " << power.getVoltageRms() << " Vrms, "
        << "I = " << power.getCurrentRms() << " Arms, "
        << "P = " << power.getActivePower() << " W, "
        << "S = " << power.getApparentPower() << " VA, "
        << "Q = " << power.getReactivePower() << " var, "
        << "cosP = " << power.getPowerFactor() << std::endl;
}