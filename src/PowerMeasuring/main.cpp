#include <Arduino.h>
#include <Ewma.h>
#include <vector>

#define PIN_U 33
#define PIN_I 32

float calU = 1;
float calI = 15;

struct ACPower
{
    float active;
    float reactive;
    float apparent;
};

bool detectZeroCross()
{
    uint16_t newSample = analogRead(PIN_U);
    static uint16_t lastSample;
    bool zeroCross = false;

    if (newSample <= 2048 && lastSample > 2048)
        zeroCross = true;
    if (newSample >= 2048 && lastSample < 2048)
        zeroCross = true;

    lastSample = newSample;
    
    return zeroCross;
}

float calcZero(std::vector<float> samples)
{
    float sum;
    for(const auto& sample : samples)
        sum += sample;
    return sum / samples.size();
}


void measure(size_t numPeriods = 1, size_t calPhi = 0)
{
    Ewma ewmaI(0.05, 0.0);
    std::vector<float> samplesU;
    std::vector<float> samplesI;
    samplesU.reserve(100 * numPeriods);
    samplesI.reserve(100 * numPeriods);

    uint32_t startMicros = micros();
    while(micros() - startMicros < 20000 * numPeriods)
    {
        samplesU.push_back(analogRead(PIN_U) * calU);
        samplesI.push_back(analogRead(PIN_I) * calI);
    }

    float zeroU = calcZero(samplesU);
    float zeroI = calcZero(samplesI);

    // Serial.printf("%f, %f\n", zeroU, zeroI);
    
    for(size_t i = 0; i < samplesU.size(); i++)
    {
        float voltage = samplesU[i] - zeroU;
        float current = ewmaI.filter(samplesI[(i + calPhi) % samplesI.size()] - zeroI);
        Serial.printf("%f, %f\n", voltage, current);
        // Serial.printf("%f\n", current);
    }
    
}

void setup()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
}

void loop()
{
    // Serial.println(analogRead(PIN_I));
    measure(5,- 10);
    uint32_t startMillis = millis();
    while(millis() - startMillis < 50)
    {
        Serial.printf("0, 0\n");
    }
}
























































// #include <EnovaEmonLib.h>

// EnergyMonitor emon;

// void runCLI()
// {
//     float calibrationPhase = 0;
//     Serial.println("Enter calibrationPhase");
//     while(calibrationPhase == 0)
//     {
//         calibrationPhase = Serial.parseFloat();
//     }

//     float calibrationV = 0;
//     Serial.println("Enter calibrationV");
//     while(calibrationV == 0)
//     {
//         calibrationV = Serial.parseFloat();
//     }

//     float calibrationI = 0;
//     Serial.println("Enter calibrationI");
//     while(calibrationI == 0)
//     {
//         calibrationI = Serial.parseFloat();
//     }

//     Serial.printf("calibrationV: %f\ncalibrationI: %f\ncalibrationPhase: %f\n", calibrationV, calibrationI, calibrationPhase);

//     emon.voltage(33, calibrationV, calibrationPhase);
//     emon.current(32, calibrationI);
//     emon.calcVI(50, 5000, 500);
//     emon.serialprint();
// }

// // void runAutoCal(float targetV, float targetI, float targetPF, float calibrationV = 0, float calibrationI = 0, float calibrationPhase = 0)
// // {
// //     while(emon.Vrms < targetV - 5 || emon.Vrms > targetV + 5)
// //     {
// //         Serial.printf("Trying calibrationV: %f\n", calibrationV);
// //         emon.voltage(33, calibrationV, 0);
// //         emon.calcVI(50, 2000);
// //         calibrationV += 2;
// //         Serial.printf("Voltage is: %f V\n", emon.Vrms);

// //     }
// //     Serial.println("Voltage Calibratet");

// //     while(emon.Vrms < targetI - 5 || emon.Vrms > targetI + 5)
// //     {
// //         Serial.printf("Trying calibrationI: %f\n", calibrationI);
// //         emon.voltage(33, calibrationI, 0);
// //         emon.calcVI(50, 2000);
// //         calibrationI += 0.5;
// //         Serial.printf("Current is: %f V\n", emon.Irms);

// //     }
// //     Serial.println("Current Calibratet");
// // }

// void setup()
// {
//     Serial.begin(115200);
//     pinMode(2, OUTPUT);
//     digitalWrite(2, HIGH);
//     emon.voltage(33, 1225, 53);
//     emon.current(32, 2);
// }

// void loop()
// {
//     // runAutoCal(236, 0.283, 1, 950, 15);
//     // runCLI();
//     emon.calcVI(20, 2000, 500);
//     emon.serialprint();
// }