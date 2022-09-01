#include <AccumulationAverage.h>

float values1[] = {12.4f, 932.25f, 43.0f, 88.19f};
float values2[] = {33.42f, 234.99f, 9001.3f, 54.33f, 12.4f, 90.73f};

AccumulationAverage<float> average;

void setup()
{
  Serial.begin(9600);
  
  // Calculate Average over array [values1]
  for(float value : values1)
  {
    average.add(value);
  }
  Serial.print("Average of [values1] is: ");
  Serial.println(average.get());

  // Reset the Average
  average.reset();


  // Calculate Average over array [values2]
  for(float value : values2)
  {
    average.add(value);
  }
  Serial.print("Average of [values2] is: ");
  Serial.println(average.get());
}

void loop()
{}