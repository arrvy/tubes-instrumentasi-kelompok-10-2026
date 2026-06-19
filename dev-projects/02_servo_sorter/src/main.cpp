#include <Arduino.h>

#include "ServoSorter.h"
#include "ServoSorterConfig.h"

ServoSorter::Config servoCfg
{
    .servoPin = 18,

    .homeAngle = 90,

    .redAngle = 30,
    .greenAngle = 60,
    .blueAngle = 120,
    .whiteAngle = 150,
    .blackAngle = 180,

    .sortDelayMs = 1500,

    .holdTimeMs = 500
};

ServoSorter servo(servoCfg);

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