#include <Arduino.h>

/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

TaskHandle_t Task1;
TaskHandle_t Task2;

// LED pins
const int led1 = 2;
const int led2 = 4;

int globalResource = 0;
StackType_t taskStack[20000];

// Task1code: blinks an LED every 1000 ms
void Task1code(void *pvParameters)
{
    for (;;)
    {
        globalResource++;
        delay(1000);
    }
}

// Task2code: blinks an LED every 700 ms
void Task2code(void *pvParameters)
{
    for (;;)
    {
        Serial.println(globalResource);
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);


    xTaskCreateStaticPinnedToCore(Task1code, "Task1", 5000, nullptr, 2, taskStack, nullptr, 1);
    // xTaskCreateStaticPinnedToCore(Task2code, "Task2", 5000, nullptr, 1, taskStack, nullptr, 1);
}

void loop()
{
}