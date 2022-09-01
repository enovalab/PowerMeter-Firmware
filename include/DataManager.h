#ifndef DataManager_h
#define DataManager_h

#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <AccumulationAverage.h>
#include <EmonLib.h>

class DataManager
{
    private:
    const char* filePath;
    DynamicJsonDocument data = DynamicJsonDocument(30000);
    AccumulationAverage<float> average;
    EnergyMonitor& emon;

    float averageJsonArray(const JsonArray& array) const
    {   
        float sum;
        for(auto item : array) sum += item["Value"].as<float>();
        return sum / array.size();
    }

    public:
    DataManager(EnergyMonitor& emon, const char* filePath) : emon(emon), filePath(filePath)
    {
        SPIFFS.begin();
    }

    void push()
    {
        static uint32_t lastPush;
        if(millis() - lastPush >= 1000)
        {
            emon.calcVI(20, 2000);
            average += emon.realPower;

            if(/* Stunde vorbei */)
            {
                File dataFile = SPIFFS.open(filePath);
                deserializeJson(data, dataFile);

                JsonObject newJsonData;
                newJsonData["Value"] = average.get();
                average.reset();
                newJsonData["Timestamp"] = /* Timestamp holen */;
                
                if(data["LastDay"].size() >= 24) data["LastDay"].remove(0);
                data["LastDay"].add(newJsonData);

                serializeJson(data, dataFile);
                dataFile.close();
            }

            if(/* Tag vorbei */)
            {
                File dataFile = SPIFFS.open(filePath);
                deserializeJson(data, dataFile);

                JsonObject newJsonData;
                newJsonData["Value"] = averageJsonArray(data["LastDay"]);
                newJsonData["Timestamp"] = /* Timestamp holen */;
                
                if(data["LastWeek"].size() >= 7) data["LastWeek"].remove(0);
                data["LastWeek"].add(newJsonData);

                if(data["LastMonth"].size() >= 30) data["LastMonth"].remove(0);
                data["LastMonth"].add(newJsonData);

                serializeJson(data, dataFile);
                dataFile.close();
            }

            if(/* Monat vorbei */)
            {
                File dataFile = SPIFFS.open(filePath);
                deserializeJson(data, dataFile);

                JsonObject newJsonData;
                newJsonData["Value"] = averageJsonArray(data["LastMonth"]);
                newJsonData["Timestamp"] = /* Timestamp holen */;
                
                if(data["LastYear"].size() >= 12) data["LastYear"].remove(0);
                data["LastYear"].add(newJsonData);

                serializeJson(data, dataFile);
                dataFile.close();
            }

            if(/* Jahr vorbei */)
            {
                File dataFile = SPIFFS.open(filePath);
                deserializeJson(data, dataFile);

                JsonObject newJsonData;
                newJsonData["Value"] = averageJsonArray(data["LastYear"]);
                newJsonData["Timestamp"] = /* Timestamp holen */;
                
                if(data["LastDecate"].size() >= 12) data["LastYear"].remove(0);
                data["LastDecade"].add(newJsonData);

                serializeJson(data, dataFile);
                dataFile.close();
            }
        }
    }
};


#endif