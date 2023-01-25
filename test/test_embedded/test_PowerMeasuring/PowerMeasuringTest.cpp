#include "Measuring/ACPowerMeter.h"
#include "Connectivity/RestAPI.h"
#include "Diagnostics/Log.h"

constexpr uint8_t voltagePin = 33;
constexpr uint8_t currentPin = 32;

constexpr float voltageCalibration = 0.4908f;
constexpr float currentCalibration = 0.01596f;
constexpr int16_t phaseCalibration = 12;


Measuring::ACPowerMeter powerMeter(voltagePin, currentPin);
AsyncWebServer server(80);


void setup()
{
    Serial.begin(115200);
    
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("Power Measuring Test", "Katek1976");
    Diagnostics::Logger[Level::Info] << "AP IP Address is: " << WiFi.softAPIP().toString().c_str() << std::endl;

    powerMeter.calibrate(voltageCalibration, currentCalibration, phaseCalibration);
    
    Connectivity::RestAPI api(&server);
    
    api.handle(Connectivity::HTTP::Method::Get, "", [](json){
        Measuring::ACPower power = powerMeter.measure();
        json data;
        data["voltage"] = power.getVoltageRms();
        data["current"] = power.getCurrentRms();
        data["active"] = power.getActivePower();
        data["apparent"] = power.getApparentPower();
        data["reactive"] = power.getReactivePower();
        data["powerFactor"] = power.getPowerFactor();
        return Connectivity::RestAPI::JsonResponse(data);
    });

    server.begin();
}


void loop()
{
    Measuring::ACPower power = powerMeter.measure();
    Diagnostics::Logger[Level::Info]
        << "U = " << power.getVoltageRms() << " Vrms, "
        << "I = " << power.getCurrentRms() << " Arms, "
        << "P = " << power.getActivePower() << " W, "
        << "S = " << power.getApparentPower() << " VA, "
        << "Q = " << power.getReactivePower() << " var, "
        << "cosP = " << power.getPowerFactor() << std::endl;
    delay(1000);
}