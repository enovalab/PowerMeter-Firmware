#ifdef ESP32

#include "Device/PowerMeter.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"
#include "Measuring/ACPowerMeter.h"
#include "Time/DS3231.h"
#include "Data/Tracker.h"
#include "Connectivity/RestAPI.h"

#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <LittleFS.h>
#include <fstream>

using namespace Device;

namespace
{
    Measuring::ACPowerMeter powerMeter(33, 32);
    Time::DS3231 rtc;
    Data::Tracker tracker(rtc);
    AsyncWebServer server(80);
    AsyncWebSocket powerWebsocket("/ws/power");
    Connectivity::RestAPI api(&server, "/api", {
        Connectivity::HTTP::Header("Access-Control-Request-Method", "*"),
        Connectivity::HTTP::Header("Access-Control-Expose-Headers", "*"),
        Connectivity::HTTP::Header("Access-Control-Allow-Methods", "*"),
        Connectivity::HTTP::Header("Access-Control-Allow-Origin", "*"),
        Connectivity::HTTP::Header("Access-Control-Allow-Headers", "*")
    });

    Connectivity::RestAPI::JsonResponse handleGetJsonURI(const Data::JsonURI& jsonURI)
    {
        return Connectivity::RestAPI::JsonResponse(jsonURI.deserialize());
    }

    Connectivity::RestAPI::JsonResponse handlePutJsonURI(const Data::JsonURI& jsonURI, const json& data)
    {
        jsonURI.serialize(data);
        return Connectivity::RestAPI::JsonResponse(jsonURI.deserialize());
    }

    Connectivity::RestAPI::JsonResponse handlePatchJsonURI(const Data::JsonURI& jsonURI, const json& data)
    {
        json storedData = jsonURI.deserialize();
        size_t sizeBefore = storedData.size();
        storedData.update(data, true);

        if(storedData.size() > sizeBefore)
            throw std::runtime_error("The PowerMeter API does not allow adding properties using PATCH");

        jsonURI.serialize(storedData);
        return Connectivity::RestAPI::JsonResponse(jsonURI.deserialize());
    }

    void configureLogger(const Data::JsonURI& loggerConfigURI)
    {
        try
        {
            Diagnostics::Logger[Level::Info] << "Configuring Logger..." << std::endl;
            json loggerConfigJson = loggerConfigURI.deserialize();

            Serial.begin(loggerConfigJson.at("baudRate"));
            Diagnostics::Logger.setLevel(Diagnostics::Log::getLevelByName(loggerConfigJson.at("level")));
            Diagnostics::Logger.setShowLevel(loggerConfigJson.at("showLevel"));
            const std::string& logFilePath = loggerConfigJson.at("logFile");

            static std::ofstream logFile;
            if (loggerConfigJson.at("fileMode"))
            {
                logFile.open(logFilePath);
                Diagnostics::Logger.setOutputStream(&logFile);
            }

            server.on("/log", HTTP_GET, [logFilePath](AsyncWebServerRequest *request){
                logFile.close();
                request->send(LittleFS, logFilePath.substr(9).c_str(), "text/plain");
                logFile.open(logFilePath, std::ios::app); 
            });

            api.handle(Connectivity::HTTP::Method::Get, "/config/logger", std::bind(handleGetJsonURI, loggerConfigURI));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/logger", [loggerConfigURI](const json& data){
                Connectivity::RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(loggerConfigURI, data);
                configureLogger(loggerConfigURI);
                return jsonResponse; 
            });

            Diagnostics::Logger[Level::Info] << "Logger configured sucessfully." << std::endl;
        }
        catch (...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Diagnostics::Logger' from \"" << loggerConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureMeasuring(const Data::JsonURI& measuringConfigURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring measuring unit..." << std::endl;
        try
        {
            json powerMeterConfigJson = measuringConfigURI.deserialize();

            uint16_t pinU = powerMeterConfigJson.at("/pins/voltage"_json_pointer);
            uint16_t pinI = powerMeterConfigJson.at("/pins/current"_json_pointer);

            powerMeter = Measuring::ACPowerMeter(pinU, pinI);

            float calU = powerMeterConfigJson.at("/calibration/voltage"_json_pointer);
            float calI = powerMeterConfigJson.at("/calibration/current"_json_pointer);
            float calPhase = powerMeterConfigJson.at("/calibration/phase"_json_pointer);

            powerMeter.calibrate(calU, calI, calPhase);

            api.handle(Connectivity::HTTP::Method::Get, "/power", [](json){
                json data;
                Measuring::ACPower power = powerMeter.measure();
                data["voltage"] = power.getVoltageRms();
                data["current"] = power.getCurrentRms();
                data["active"] = power.getActivePower();
                data["apparent"] = power.getApparentPower();
                data["reactive"] = power.getReactivePower();
                data["powerFactor"] = power.getPowerFactor();
                return Connectivity::RestAPI::JsonResponse(data);
            });
            api.handle(Connectivity::HTTP::Method::Get, "/config/measuring", std::bind(handleGetJsonURI, measuringConfigURI));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/measuring", [measuringConfigURI](const json& data){
                Connectivity::RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(measuringConfigURI, data);
                configureMeasuring(measuringConfigURI);
                return jsonResponse; 
            });

            Diagnostics::Logger[Level::Info] << "Measuring unit configured sucessfully." << std::endl;
        }
        catch (...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Measuring::PowerMeter' from \"" << measuringConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureRelay(const Data::JsonURI& relayConfigURI)
    {
        try
        {
            Diagnostics::Logger[Level::Info] << "Configuring relay..." << std::endl;

            json relayConfigJson = relayConfigURI.deserialize();
            const uint16_t& pin = relayConfigJson.at("pin");
            ;
            bool state = relayConfigJson.at("state");
            pinMode(pin, OUTPUT);
            digitalWrite(pin, state);

            api.handle(Connectivity::HTTP::Method::Get, "/config/relay", std::bind(handleGetJsonURI, relayConfigURI));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/relay", [relayConfigURI](const json& data){
                Connectivity::RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(relayConfigURI, data);
                json relayConfigJson = relayConfigURI.deserialize();
                const uint16_t& pin = relayConfigJson.at("pin");;
                bool state = relayConfigJson.at("state");
                pinMode(pin, OUTPUT);
                digitalWrite(pin, state);
                return jsonResponse; 
            });

            Diagnostics::Logger[Level::Info] << "Relay configured sucessfully." << std::endl;
        }
        catch (...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure Relay from \"" << relayConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureTracker(const Data::JsonURI& trackerConfigURI)
    {
        try
        {
            Diagnostics::Logger[Level::Info] << "Configuring tracker..." << std::endl;
            json jsonTrackingSpans = trackerConfigURI.deserialize();
            std::vector<Data::TrackingSpan> trackingSpans;

            for(const auto &jsonTrackingSpan : jsonTrackingSpans.items())
            {
                const Data::JsonURI &targetURI = Data::JsonURI(jsonTrackingSpan.value().at("targetURI"));
                const Data::JsonURI &lastSampleURI = Data::JsonURI(jsonTrackingSpan.value().at("lastSampleURI"));
                uint32_t timeSpanSeconds = jsonTrackingSpan.value().at("timeSpanSeconds");
                uint32_t numSamplesPerSpan = jsonTrackingSpan.value().at("numSamplesPerSpan");
                const Data::JsonURI &averageURI = Data::JsonURI(jsonTrackingSpan.value().value("averageURI", ""));

                trackingSpans.push_back(Data::TrackingSpan(targetURI, lastSampleURI, timeSpanSeconds, numSamplesPerSpan, averageURI));

                api.handle(
                    Connectivity::HTTP::Method::Get,
                    std::string("/tracker/") + jsonTrackingSpan.key(),
                    std::bind(handleGetJsonURI, Data::JsonURI(jsonTrackingSpan.value().at("targetURI")))
                );
            }

            api.handle(Connectivity::HTTP::Method::Get, "/tracker", [jsonTrackingSpans](json){
                json jsonTrackerData;
                for(const auto &jsonTrackingSpan : jsonTrackingSpans.items())
                    jsonTrackerData[jsonTrackingSpan.key()] = Data::JsonURI(jsonTrackingSpan.value().at("targetURI")).deserialize();
                // Diagnostics::Logger[Level::Debug] << jsonTrackerData.dump(1, ' ') << std::endl;
                return Connectivity::RestAPI::JsonResponse(jsonTrackerData);
            });

            api.handle(Connectivity::HTTP::Method::Put, "/tracker", [jsonTrackingSpans](const json& jsonTrackerData){

                for(const auto &jsonTrackingSpan : jsonTrackingSpans.items())
                    Data::JsonURI(jsonTrackingSpan.value().at("targetURI")).serialize(jsonTrackerData.at(jsonTrackingSpan.key()));
                return Connectivity::RestAPI::JsonResponse(jsonTrackerData);
            });

            tracker.setTrackingSpans(trackingSpans);
            tracker.init();

            api.handle(Connectivity::HTTP::Method::Get, "/config/tracker", std::bind(handleGetJsonURI, trackerConfigURI));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/tracker", [trackerConfigURI](const json &data){
                Connectivity::RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(trackerConfigURI, data);
                configureTracker(trackerConfigURI);
                return jsonResponse; 
            });

            Diagnostics::Logger[Level::Info] << "Tracker configured sucessfully." << std::endl;
        }
        catch (...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Data::Tracker' from \"" << trackerConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    bool configureWifiStationary(const Data::JsonURI& wifiConfigURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring WiFi in stationary mode..." << std::endl;
        Data::JsonURI staConfigURI = wifiConfigURI / "/sta"_json_pointer;
        try
        {
            json staConfigJson = staConfigURI.deserialize();

            const std::string& ssid = staConfigJson.at("ssid");
            const std::string& password = staConfigJson.at("password");
            const std::string& staticIP = staConfigJson.at("staticIP");
            const std::string& gateway = staConfigJson.at("gateway");
            const std::string& subnet = staConfigJson.at("subnet");

            auto parseIP = [](const std::string& address)
            {
                IPAddress ip;
                if (!ip.fromString(address.c_str()))
                    Diagnostics::Logger[Level::Warning] << SOURCE_LOCATION << "Could not convert \"" << address << "\" to 'IPAdress'" << std::endl;
                return ip;
            };
            
            WiFi.disconnect(true);
            WiFi.config(parseIP(staticIP), parseIP(gateway), parseIP(subnet));
            WiFi.begin(ssid.c_str(), password.c_str());
            WiFi.setSleep(false);

            if(WiFi.waitForConnectResult(3000) == WL_CONNECTED)
                return true;
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure WiFi in Stationary Mode from \"" << staConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
        return false;
    }


    bool configureWifiAccesspoint(const Data::JsonURI& wifiConfigURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring WiFi as accesspoint..." << std::endl;
        Data::JsonURI apConfigURI = wifiConfigURI / "/ap"_json_pointer;
        try
        {
            json apConfigJson = apConfigURI.deserialize();
            const std::string& ssid = apConfigJson.at("ssid");
            const std::string& password = apConfigJson.at("password");

            return WiFi.softAP(ssid.c_str(), password.c_str());
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure WiFi as accesspoint from \"" << apConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureWifi(const Data::JsonURI& wifiConfigURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring WiFi..." << std::endl;
        
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

        api.handle(Connectivity::HTTP::Method::Get, "/config/wifi", [wifiConfigURI, wifiModeToString](json){
            Connectivity::RestAPI::JsonResponse jsonResponse = handleGetJsonURI(wifiConfigURI);
            jsonResponse.data.at("sta").erase("password");
            jsonResponse.data["mode"] = wifiModeToString(WiFi.getMode());
            jsonResponse.data["sta"]["ip"] = WiFi.localIP().toString().c_str();
            jsonResponse.data["ap"]["ip"] = WiFi.softAPIP().toString().c_str();
            return jsonResponse;
        });

        api.handle(Connectivity::HTTP::Method::Patch, "/config/wifi", [wifiConfigURI, wifiModeToString](const json& data){
            Connectivity::RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(wifiConfigURI, data);
            configureWifi(wifiConfigURI);
            jsonResponse.data.at("sta").erase("password");
            jsonResponse.data["mode"] = wifiModeToString(WiFi.getMode());
            jsonResponse.data["sta"]["ip"] = WiFi.localIP().toString().c_str();
            jsonResponse.data["ap"]["ip"] = WiFi.softAPIP().toString().c_str();
            return jsonResponse;
        });

        try
        {
            WiFi.mode(WiFiMode_t::WIFI_MODE_STA);
            if(configureWifiStationary(wifiConfigURI))
            {
                Diagnostics::Logger[Level::Info] 
                    << "WiFi configured sucessfully in Stationary Mode. " 
                    << "IP: http://" << WiFi.localIP().toString().c_str()
                    << std::endl;
                return;
            }
            Diagnostics::Logger[Level::Info] << "Couldn't connect to local Network. Setting up Acesspoint..." << std::endl;
            WiFi.mode(WiFiMode_t::WIFI_MODE_AP);
            if(configureWifiAccesspoint(wifiConfigURI))
            {
                Diagnostics::Logger[Level::Info] 
                    << "WiFi configured sucessfully in Acesspoint Mode. "
                    << "IP: http://" << WiFi.softAPIP().toString().c_str()
                    << std::endl;
                return;
            }

            throw std::runtime_error("Failed to configure WiFi");        
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure WiFi from \"" << wifiConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }
}


bool PowerMeter::boot() noexcept
{
    bool success = true;
    Serial.begin(115200);
    LittleFS.begin(POWERMETER_FORMAT_FS_ON_FAIL, "/littlefs", 30);

    AsyncElegantOTA.begin(&server);
    server.serveStatic("/", LittleFS, "/app").setDefaultFile("index.html");
    
    api.handle(Connectivity::HTTP::Method::Get, "/reboot", [](json){
        Diagnostics::Logger[Level::Info] << "Rebooting Power Meter..." << std::endl;
        ESP.restart();
        return Connectivity::RestAPI::JsonResponse(nullptr, Connectivity::HTTP::StatusCode::NoContent); 
    });

    api.handle(Connectivity::HTTP::Method::Get, "/info", [](json){
        json data;
        data["mac"] = ESP.getEfuseMac();
        data["firmware"] = POWERMETER_FIRMWARE_VERSION;
        data["uptimeMilliseconds"] = millis();
        data["filesystem"]["totalBytes"] = LittleFS.totalBytes();
        data["filesystem"]["usedBytes"] = LittleFS.usedBytes();
        data["heap"]["totalBytes"] = ESP.getHeapSize();
        data["heap"]["usedBytes"] = ESP.getHeapSize() - ESP.getFreeHeap();
        return Connectivity::RestAPI::JsonResponse(data); 
    });

    try
    {
        configureLogger(Data::JsonURI(POWERMETER_LOGGER_CONFIG_URI));
        Diagnostics::Logger[Level::Info] << "Booting..." << std::endl;
        configureWifi(Data::JsonURI(POWERMETER_WIFI_CONFIG_URI));
        configureMeasuring(Data::JsonURI(POWERMETER_MEASURING_CONFIG_URI));
        configureRelay(Data::JsonURI(POWERMETER_RELAY_CONFIG_URI));
        rtc.begin();
        configureTracker(Data::JsonURI(POWERMETER_TRACKER_CONFIG_URI));
    }
    catch(...)
    {
        Diagnostics::Logger[Level::Error] 
            << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << Diagnostics::ExceptionTrace::what() << std::endl;
        success = false;
    }

    Diagnostics::Logger[Level::Info] << "Boot sequence finished. Running..." << std::endl;

    server.addHandler(&powerWebsocket);
    server.begin();
    return success;
}

void PowerMeter::run() noexcept
{
    try
    {
        Measuring::ACPower power = powerMeter.measure();

        tracker.track(power.getActivePower());

        json powerJson;
        powerJson["voltage"] = power.getVoltageRms();
        powerJson["current"] = power.getCurrentRms();
        powerJson["active"] = power.getActivePower();
        powerJson["apparent"] = power.getApparentPower();
        powerJson["reactive"] = power.getReactivePower();
        powerJson["powerFactor"] = power.getPowerFactor();
        powerWebsocket.textAll(powerJson.dump().c_str());
        powerWebsocket.cleanupClients();

        delay(500);
    }
    catch(...)
    {
        Diagnostics::Logger[Level::Error] 
            << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << Diagnostics::ExceptionTrace::what() << std::endl;
    }
}

#endif