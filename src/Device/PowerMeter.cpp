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
#include <unordered_map>
#include <dirent.h>

using namespace Device;

namespace
{
    Measuring::ACPowerMeter powerMeter(33, 32);
    Time::DS3231 rtc;
    std::unordered_map<std::string, Data::Tracker> trackers;
    AsyncWebServer server(80);
    Connectivity::RestAPI api(&server, "/api", {
        Connectivity::HTTP::Header("Access-Control-Request-Method", "*"),
        Connectivity::HTTP::Header("Access-Control-Expose-Headers", "*"),
        Connectivity::HTTP::Header("Access-Control-Allow-Methods", "*"),
        Connectivity::HTTP::Header("Access-Control-Allow-Origin", "*"),
        Connectivity::HTTP::Header("Access-Control-Allow-Headers", "*")
    });


    void printDirectoryHierarchy(const std::string& directoryPath = "", int level = 0)
    {
        DIR* dir;
        struct dirent* entry;

        if ((dir = opendir(directoryPath.c_str())) == nullptr) {
            std::cout << "Invalid directory path: " << directoryPath << std::endl;
            return;
        }

        while ((entry = readdir(dir)) != nullptr) {
            if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
                continue;
            }

            std::string fullPath = directoryPath + "/" + entry->d_name;

            if (entry->d_type == DT_DIR) {
                // Print the current directory with appropriate indentation
                std::cout << std::string(level, '\t') << "[+] " << entry->d_name << std::endl;

                // Recursively call the function for subdirectories
                printDirectoryHierarchy(fullPath, level + 1);
            } else {
                // Print files within the directory with indentation
                std::cout << std::string(level, '\t') << " - " << entry->d_name << std::endl;
            }
        }

        closedir(dir);
    }


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
        storedData.merge_patch(data);

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
                data["voltageRMS_V"] = power.getVoltageRMS_V();
                data["currentRMS_A"] = power.getCurrentRMS_A();
                data["activePower_W"] = power.getActivePower_W();
                data["apparentPower_VA"] = power.getApparentPower_VA();
                data["reactivePower_var"] = power.getReactivePower_var();
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
            uint16_t pin = relayConfigJson.at("pin");
            Data::JsonURI relayStateURI(POWERMETER_RELAY_STATE_URI);
            bool state = relayStateURI.deserialize();
            pinMode(pin, OUTPUT);
            digitalWrite(pin, state);

            api.handle(Connectivity::HTTP::Method::Get, "/config/relay", std::bind(handleGetJsonURI, relayConfigURI));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/relay", [relayConfigURI](const json& data){
                Connectivity::RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(relayConfigURI, data);
                configureRelay(relayConfigURI);
                return jsonResponse; 
            });
            api.handle(Connectivity::HTTP::Method::Get, "/relay", std::bind(handleGetJsonURI, relayStateURI));
            api.handle(Connectivity::HTTP::Method::Patch, "/relay", [relayStateURI, pin](const json& data){
                Connectivity::RestAPI::JsonResponse jsonResponse = handlePatchJsonURI(relayStateURI, data);
                digitalWrite(pin, data);
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


    void configureTrackers(const Data::JsonURI& trackerConfigURI)
    {
        try
        {
            Diagnostics::Logger[Level::Info] << "Configuring trackers..." << std::endl;

            json jsonTrackers = trackerConfigURI.deserialize();
            for(const auto& jsonTracker : jsonTrackers.items())
            {
                std::string key = jsonTracker.key();
                std::stringstream trackerDirectory;
                trackerDirectory << POWERMETER_TRACKERS_DIRECTORY << key << '/';
                Diagnostics::Logger[Level::Debug] << trackerDirectory.str() << std::endl;

                trackers.emplace(key, Data::Tracker(
                    jsonTracker.value().at("title"),
                    jsonTracker.value().at("duration_s"),
                    jsonTracker.value().at("sampleCount"),
                    rtc,
                    Data::JsonURI(trackerDirectory.str() + "data.json"),
                    Data::JsonURI(trackerDirectory.str() + "timestamps.json#/lastInput"),
                    Data::JsonURI(trackerDirectory.str() + "timestamps.json#/lastSample"),
                    Data::AverageAccumulator(Data::JsonURI(trackerDirectory.str() + "accumulator.json"))
                ));

                std::stringstream configURL;
                configURL << "/config/trackers/" << key;
                api.handle(Connectivity::HTTP::Method::Delete, configURL.str(), [trackerConfigURI, key](json){
                    Diagnostics::Logger[Level::Debug] << "handling Delete" << std::endl;
                    json configJson = trackerConfigURI.deserialize();
                    trackers.at(key).erase();
                    trackers.erase(key);
                    configJson.erase(key);
                    trackerConfigURI.serialize(configJson);
                    return Connectivity::RestAPI::JsonResponse(configJson);
                });
            }

            api.handle(Connectivity::HTTP::Method::Get, "/config/trackers", std::bind(handleGetJsonURI, trackerConfigURI));
            api.handle(Connectivity::HTTP::Method::Post, "/config/trackers", [trackerConfigURI](const json& data){
                json configJson = trackerConfigURI.deserialize();
                std::stringstream key;
                key << data.at("duration_s") << "_" << data.at("sampleCount");
                configJson[key.str()] = data;
                trackerConfigURI.serialize(configJson);

                configureTrackers(trackerConfigURI);
                return Connectivity::RestAPI::JsonResponse(configJson, Connectivity::HTTP::StatusCode::Created);
            });

            api.handle(Connectivity::HTTP::Method::Get, "/trackers", [](json){
                json data = json::object_t();
                for(const auto& tracker : trackers)
                    data[tracker.first] = tracker.second.getData();

                return Connectivity::RestAPI::JsonResponse(data);
            });

            api.handle(Connectivity::HTTP::Method::Put, "/trackers", [](const json& data){
                for(const auto& dataItems : data.items())
                    trackers.at(dataItems.key()).setData(dataItems.value());
                return Connectivity::RestAPI::JsonResponse(data);
            });

            Diagnostics::Logger[Level::Info] << "Trackers configured sucessfully." << std::endl;
        }
        catch (...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure trackers from \"" << trackerConfigURI << '"';
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
            json wifiConfigJson = wifiConfigURI.deserialize();

            WiFi.mode(WiFiMode_t::WIFI_MODE_STA);
            if(configureWifiStationary(wifiConfigURI))
            {
                Diagnostics::Logger[Level::Info] 
                    << "WiFi connected sucessfully to '"
                    << wifiConfigJson.at("sta").at("ssid")
                    << "'. " 
                    << "IP: http://" << WiFi.localIP().toString().c_str()
                    << std::endl;
                return;
            }
            Diagnostics::Logger[Level::Info] << "Couldn't connect to" << wifiConfigJson.at("sta").at("ssid") << ". Setting up Acesspoint..." << std::endl;
            WiFi.mode(WiFiMode_t::WIFI_MODE_AP);
            if(configureWifiAccesspoint(wifiConfigURI))
            {
                Diagnostics::Logger[Level::Info] 
                    << "WiFi configured sucessfully in Acesspoint Mode. Network Name: "
                    << wifiConfigJson.at("ap").at("ssid")
                    << " IP: http://" << WiFi.softAPIP().toString().c_str()
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
    LittleFS.begin(POWERMETER_FORMAT_FS_ON_FAIL, "", 30);

    AsyncElegantOTA.begin(&server);
    
    api.handle(Connectivity::HTTP::Method::Get, "/reboot", [](json){
        Diagnostics::Logger[Level::Info] << "Rebooting Power Meter..." << std::endl;
        ESP.restart();
        return Connectivity::RestAPI::JsonResponse(nullptr, Connectivity::HTTP::StatusCode::NoContent); 
    });

    api.handle(Connectivity::HTTP::Method::Get, "/info", [](json){
        json data;
        data["mac"] = ESP.getEfuseMac();
        data["firmware"] = POWERMETER_FIRMWARE_VERSION;
        data["uptime_ms"] = millis();
        data["filesystem"]["total_B"] = LittleFS.totalBytes();
        data["filesystem"]["used_B"] = LittleFS.usedBytes();
        data["heap"]["total_B"] = ESP.getHeapSize();
        data["heap"]["used_B"] = ESP.getHeapSize() - ESP.getFreeHeap();
        return Connectivity::RestAPI::JsonResponse(data); 
    });

    try
    {
        printDirectoryHierarchy();
        configureLogger(Data::JsonURI(POWERMETER_LOGGER_CONFIG_URI));
        Diagnostics::Logger[Level::Info] << "Booting..." << std::endl;
        configureWifi(Data::JsonURI(POWERMETER_WIFI_CONFIG_URI));
        configureMeasuring(Data::JsonURI(POWERMETER_MEASURING_CONFIG_URI));
        configureRelay(Data::JsonURI(POWERMETER_RELAY_CONFIG_URI));
        rtc.begin();
        configureTrackers(Data::JsonURI(POWERMETER_TRACKERS_CONFIG_URI));
    }
    catch(...)
    {
        Diagnostics::Logger[Level::Error] 
            << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << Diagnostics::ExceptionTrace::what() << std::endl;
        success = false;
    }

    Diagnostics::Logger[Level::Info] << "Boot sequence finished. Running..." << std::endl;
    server.begin();
    return success;
}

void PowerMeter::run() noexcept
{
    try
    {
        Measuring::ACPower power = powerMeter.measure();

        // Diagnostics::Logger[Level::Debug]
        //     << "U = " << power.getVoltageRMS_V() << " Vrms, "
        //     << "I = " << power.getCurrentRMS_A() << " Arms, "
        //     << "P = " << power.getActivePower_W() << " W, "
        //     << "S = " << power.getApparentPower_VA() << " VA, "
        //     << "Q = " << power.getReactivePower_var() << " var, "
        //     << "cosP = " << power.getPowerFactor() << std::endl;

        uint32_t millisBeforeTracking = millis();
        for(auto& tracker : trackers)
            tracker.second.track(power.getActivePower_W());
        // Diagnostics::Logger[Level::Debug] << "Tracking took " << millis() - millisBeforeTracking << "ms" << std::endl;

        // printDirectoryHierarchy();
    
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