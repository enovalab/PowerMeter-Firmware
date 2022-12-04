#include "Measuring/PowerMeter.h"
#include "Measuring/PowerMeter.cpp"

#define CAL_U 0.4908f
#define CAL_I 0.01596f
#define CAL_PHASE 12
#define PIN_U 33
#define PIN_I 32
#define PIN_RELAY 2

PowerMeter meter(PIN_U, PIN_I);

void setup()
{
    Serial.begin(115200);
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, HIGH);
    meter.calibrate(CAL_U, CAL_I, CAL_PHASE);
}

void loop()
{
    delay(1000);
    ACPower measureResult = meter.measure();
    Serial.printf("U = %fVrms, I = %fArms, P = %fW, S = %fVA, Q = %fvar, cosP = %f\n", 
        measureResult.getVoltageRms(),
        measureResult.getCurrentRms(),
        measureResult.getActivePower(),
        measureResult.getApparentPower(),
        measureResult.getReactivePower(),
        measureResult.getPowerFactor()
    );
}