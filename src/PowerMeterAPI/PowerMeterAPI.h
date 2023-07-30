#ifndef POWERMETERAPI_H
#define POWERMETERAPI_H

#include "RestAPI/RestAPI.h"
#include "JsonURI/JsonURI.h"
#include "ACPowerMeter/ACPowerMeter.h"


namespace PowerMeterAPI
{
    void createSystemEndpoints();
    void createLoggerEndpoints();
    void createMeasuringEndpoints();
    void createRelayEndpoints();
    void createTrackerEndpoints();
    void createWiFiEndpoints();
}

#endif