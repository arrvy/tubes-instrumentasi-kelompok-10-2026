#include <Arduino.h>
#include "ConveyorMotor.h"

ConveyorMotor conveyor(ConveyorMotor::defaultConfig());

void setup()
{
    Serial.begin(115200);

    conveyor.begin();

    conveyor.start();
}

void loop()
{
    conveyor.update();

    if(Serial.available())
    {
        String cmd =
        Serial.readStringUntil('\n');

        cmd.trim();

        if(cmd == "start")
        {
            conveyor.start();
        }

        else if(cmd == "stop")
        {
            conveyor.stop();
        }

        else if(cmd == "debug")
        {
            conveyor.printDebug(
                Serial
            );
        }

        else if(cmd == "emg")
        {
            conveyor.emergencyStop();
        }

        else if(cmd.startsWith("speed "))
        {
            String speedStr = cmd.substring(6);
            uint8_t speed = speedStr.toInt();
            conveyor.setSpeed(speed);
            Serial.print(F("Speed set to: "));
            Serial.println(speed);
        }

        else if(cmd == "speed")
        {
            Serial.print(F("Current speed: "));
            Serial.println(conveyor.getSpeed());
        }
    }
}