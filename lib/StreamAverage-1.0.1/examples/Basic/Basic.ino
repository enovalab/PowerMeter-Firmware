#include <Arduino.h>
#include <StreamAverage.h>

float values1[] = {12.4f, 932.25f, 43.0f, 88.19f};
float values2[] = {33.42f, 234.99f, 9001.3f, 54.33f, 12.4f, 90.73f};

StreamAverage<float> average;

void setup()
{
  Serial.begin(9600);
  
  // Calculate Average over array [values1]
  for(float value : values1)
  {
    Serial.print("After ");
    Serial.print(average.getNumValues());
    Serial.println(" values of [values1] the");
    Serial.print("average of [values1] is: ");
    Serial.println(average.add(value));
    Serial.print("maximum of [values1] is: ");
    Serial.println(average.getMax());
    Serial.print("minimum of [values1] is: ");
    Serial.println(average.getMin());
  }

  // Reset the Average
  average.reset();
  Serial.println("\n");

  // Calculate Average over array [values2]
  for(float value : values2)
  {
    Serial.print("After ");
    Serial.print(average.getNumValues());
    Serial.println(" values of [values2] the");
    Serial.print(" average of [values2] is: ");
    Serial.println(average << value);
    Serial.print("maximum of [values2] is: ");
    Serial.println(average.getMax());
    Serial.print("minimum of [values2] is: ");
    Serial.println(average.getMin());
  }
  
}

void loop()
{}