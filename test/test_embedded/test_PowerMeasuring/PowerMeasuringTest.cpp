#include "Measuring/ACPowerMeter.h"
#include "Connectivity/RestAPI.h"
#include "Diagnostics/Log.h"

#include <AsyncElegantOTA.h>

constexpr uint8_t voltagePin = 33;
constexpr uint8_t currentPin = 32;

constexpr float voltageCalibration = 536.9;
constexpr float currentCalibration = 15.7;
constexpr float phaseCalibration = -5.6;


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
    
    api.handle(Connectivity::HTTP::Method::GET, "/power", [](json){
        json data;
        power = powerMeter.measure();
        data["voltage_V"] = power.getVoltageRMS_V();
        data["current_A"] = power.getCurrentRMS_A();
        data["activePower_W"] = power.getActivePower_W();
        data["apparentPower_VA"] = power.getApparentPower_VA();
        data["reactivePower_var"] = power.getReactivePower_var();
        data["powerFactor"] = power.getPowerFactor();
        return Connectivity::RestAPI::JsonResponse(data);
    });

    api.handle(Connectivity::HTTP::Method::GET, "/toggle", [](json){
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
        << "U = " << power.getVoltageRMS_V() << " Vrms, "
        << "I = " << power.getCurrentRMS_A() << " Arms, "
        << "P = " << power.getActivePower_W() << " W, "
        << "S = " << power.getApparentPower_VA() << " VA, "
        << "Q = " << power.getReactivePower_var() << " var, "
        << "cosP = " << power.getPowerFactor() << std::endl;
}