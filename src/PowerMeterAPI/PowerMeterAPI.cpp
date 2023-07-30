#ifdef ESP32

#include "PowerMeterAPI.h"
#include "Config/Config.h"
#include "Log/Log.h"
#include "Globals/Globals.h"
#include <WiFi.h>
#include <LittleFS.h>
#include <functional>

namespace 
{
    RestAPI::JsonResponse handleGetJsonURI(const JsonURI& jsonURI)
    {
        return RestAPI::JsonResponse(jsonURI.deserialize());
    }


    RestAPI::JsonResponse handlePutJsonURI(const JsonURI& jsonURI, const json& data)
    {
        jsonURI.serialize(data);
        return RestAPI::JsonResponse(jsonURI.deserialize());
    }


    RestAPI::JsonResponse handlePatchJsonURI(const JsonURI& jsonURI, const json& data)
    {
        json storedData = jsonURI.deserialize();
        size_t sizeBefore = storedData.size();
        storedData.merge_patch(data);

        if(storedData.size() > sizeBefore)
            throw std::runtime_error("The PowerMeter API does not allow adding properties using PATCH");

        jsonURI.serialize(storedData);
        return RestAPI::JsonResponse(jsonURI.deserialize());
    }
}

void PowerMeterAPI::createSystemEndpoints()
{
    Globals::API.registerURI("/info", HTTP::Method::GET, [](json){
        json data;
        data["mac"] = ESP.getEfuseMac();
        data["firmware"] = Globals::FirmwareVersion;
        data["uptime_ms"] = millis();
        data["filesystem"]["total_B"] = LittleFS.totalBytes();
        data["filesystem"]["used_B"] = LittleFS.usedBytes();
        data["heap"]["total_B"] = ESP.getHeapSize();
        data["heap"]["used_B"] = ESP.getHeapSize() - ESP.getFreeHeap();
        return RestAPI::JsonResponse(data); 
    });

    Globals::API.registerURI("/reboot", HTTP::Method::POST, [](json){
        Logger[Level::Info] << "Rebooting PowerMeter..." << std::endl;
        ESP.restart();
        return RestAPI::JsonResponse(nullptr, HTTP::StatusCode::NoContent); 
    });
}

void PowerMeterAPI::createLoggerEndpoints()
{
    Globals::API.registerURI("/logger", HTTP::Method::GET, std::bind(handleGetJsonURI, Globals::LoggerConfigURI));
    Globals::API.registerURI("/logger", HTTP::Method::PATCH, [](const json& data){
        RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(Globals::LoggerConfigURI, data);
        Config::configureLogger();
        return jsonResponse; 
    });
}


void PowerMeterAPI::createMeasuringEndpoints()
{
    Globals::API.registerURI("/power", HTTP::Method::GET, [](json){
        json data;
        ACPower power = Globals::PowerMeter.measure();
        data["voltageRMS_V"] = power.getVoltageRMS_V();
        data["currentRMS_A"] = power.getCurrentRMS_A();
        data["activePower_W"] = power.getActivePower_W();
        data["apparentPower_VA"] = power.getApparentPower_VA();
        data["reactivePower_var"] = power.getReactivePower_var();
        data["powerFactor"] = power.getPowerFactor();
        return RestAPI::JsonResponse(data);
    });
    Globals::API.registerURI("/config/measuring", HTTP::Method::GET, std::bind(handleGetJsonURI, Globals::MeasuringConfigURI));
    Globals::API.registerURI("/config/measuring", HTTP::Method::PATCH, [](const json& data){
        RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(Globals::MeasuringConfigURI, data);
        Config::configureACPowerMeter();
        return jsonResponse;
    });
}


void PowerMeterAPI::createRelayEndpoints()
{
    json relayConfigJson = Globals::RelayConfigURI.deserialize();
    uint16_t pin = relayConfigJson.at("pin");
    Globals::API.registerURI("/relay", HTTP::Method::GET, std::bind(handleGetJsonURI, Globals::RelayStateURI));
    Globals::API.registerURI("/relay", HTTP::Method::PATCH, [pin](const json& data){
        RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(Globals::RelayStateURI, data);
        digitalWrite(pin, data);
        return jsonResponse;
    });
    Globals::API.registerURI("/config/relay", HTTP::Method::GET, std::bind(handleGetJsonURI, Globals::RelayConfigURI));
    Globals::API.registerURI("/config/relay", HTTP::Method::PATCH, [](const json& data){
        RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(Globals::RelayConfigURI, data);
        Config::configureRelay();
        return jsonResponse; 
    });
}


void PowerMeterAPI::createTrackerEndpoints()
{
    Globals::API.registerURI("/trackers", HTTP::Method::GET, [](json){
        json data = json::object_t();
        for(const auto& tracker : Globals::Trackers)
            data[tracker.first] = tracker.second.getData();

        return RestAPI::JsonResponse(data);
    });
    Globals::API.registerURI("/trackers", HTTP::Method::PUT, [](const json& data){
        for(const auto& dataItems : data.items())
            Globals::Trackers.at(dataItems.key()).setData(dataItems.value());
        return RestAPI::JsonResponse(data);
    });
    Globals::API.registerURI("/config/trackers", HTTP::Method::GET, std::bind(handleGetJsonURI, Globals::TrackersConfigURI));
    Globals::API.registerURI("/config/trackers", HTTP::Method::POST, [](const json& data){
        json configJson = Globals::TrackersConfigURI.deserialize();
        std::stringstream key;
        key << data.at("duration_s") << "_" << data.at("sampleCount");
        configJson[key.str()] = data;
        Globals::TrackersConfigURI.serialize(configJson);
        Config::configureTrackers();
        return RestAPI::JsonResponse(configJson, HTTP::StatusCode::Created);
    });
    
    json jsonTrackers = Globals::TrackersConfigURI.deserialize();
    for(const auto& jsonTracker : jsonTrackers.items())
    {
        std::string key = jsonTracker.key();
        Globals::API.registerURI(std::string("/config/trackers/") + key, HTTP::Method::DELETE, [key](json){
            json configJson = Globals::TrackersConfigURI.deserialize();
            Globals::Trackers.at(key).erase();
            Globals::Trackers.erase(key);
            configJson.erase(key);
            Globals::TrackersConfigURI.serialize(configJson);
            Config::configureTrackers();
            return RestAPI::JsonResponse(configJson);
        });
    }
}


void PowerMeterAPI::createWiFiEndpoints()
{
    auto wifiModeToString = [](WiFiMode_t mode){
        switch(mode)
        {
            case WiFiMode_t::WIFI_MODE_AP:
                return "Acesspoint";
            case WiFiMode_t::WIFI_MODE_STA:
                return "Stationary";
            case WiFiMode_t::WIFI_MODE_APSTA:
                return "Acesspoint + Stationary";
            default:
                throw std::runtime_error("Invalid WiFi mode");
        }
    };

    Globals::API.registerURI("/config/wifi", HTTP::Method::GET, [wifiModeToString](json){
        RestAPI::JsonResponse jsonResponse = handleGetJsonURI(Globals::WiFiConfigURI);
        jsonResponse.data.at("sta").erase("password");
        jsonResponse.data["mode"] = wifiModeToString(WiFi.getMode());
        jsonResponse.data["sta"]["ip"] = WiFi.localIP().toString().c_str();
        jsonResponse.data["ap"]["ip"] = WiFi.softAPIP().toString().c_str();
        return jsonResponse;
    });

    Globals::API.registerURI("/config/wifi", HTTP::Method::PATCH, [wifiModeToString](const json& data){
        RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(Globals::WiFiConfigURI, data);
        Config::configureWiFi();
        jsonResponse.data.at("sta").erase("password");
        jsonResponse.data["mode"] = wifiModeToString(WiFi.getMode());
        jsonResponse.data["sta"]["ip"] = WiFi.localIP().toString().c_str();
        jsonResponse.data["ap"]["ip"] = WiFi.softAPIP().toString().c_str();
        return jsonResponse;
    });
}

#endif