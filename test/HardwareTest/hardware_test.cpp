#include <Arduino.h>
#include <EmonLib.h>
#include <array>

#define voltagePin 32
#define currentPin 33

std::array<uint16_t, 5000> voltageBuffer;
std::array<uint16_t, 5000> currentBuffer;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  for(uint16_t i = 0; i < 5000; i++)
  {
    voltageBuffer[i] = analogRead(voltagePin);
    currentBuffer[i] = analogRead(currentPin);
  }

  for(uint16_t i = 0; i < 5000; i++)
  {
    Serial.print(voltageBuffer[i]);
    Serial.print(",");
    Serial.println(currentBuffer[i]);
  }
}