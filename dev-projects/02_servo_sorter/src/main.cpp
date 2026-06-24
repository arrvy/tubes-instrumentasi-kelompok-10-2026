#include <Arduino.h>

#include "ServoSorter.h"


ServoSorter servo(ServoSorter::defaultConfig());

void setup()
{
    Serial.begin(115200);

    servo.begin();
}

void loop()
{
    servo.update();

    if(Serial.available())
    {
        String cmd = Serial.readStringUntil('\n');

        cmd.trim();

        if(cmd == "red")
        {
            servo.triggerSort(
                ServoSorter::ColorClass::RED
            );
        }

        else if(cmd == "green")
        {
            servo.triggerSort(
                ServoSorter::ColorClass::GREEN
            );
        }

        else if(cmd == "blue")
        {
            servo.triggerSort(
                ServoSorter::ColorClass::BLUE
            );
        }

        else if(cmd == "debug")
        {
            servo.printDebug(Serial);
        }
    }
}