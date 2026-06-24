#include <Arduino.h>

#include "EmergencyStop.h"

EmergencyStop safety(EmergencyStop::defaultConfig());

void setup()
{
    Serial.begin(115200);

    safety.begin();
}

void loop()
{
    safety.update();

    if(Serial.available())
    {
        String cmd =
        Serial.readStringUntil('\n');

        cmd.trim();

        if(cmd == "debug")
        {
            safety.printDebug(
                Serial
            );
        }

        if(cmd == "reset")
        {
            safety.resetEmergency();
        }
    }
}