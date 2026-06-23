#include <Arduino.h>

#include "EmergencyStop.h"

EmergencyStop::Config safetyConfig
{
    .mq2Pin = 34,

    .estopPin = 25,

    .feedbackPin = 26,

    .mq2Threshold = 500,

    .debounceMs = 50,

    .estopActiveLow = true,

    .feedbackActiveLow = true
};

EmergencyStop safety(
    safetyConfig
);

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