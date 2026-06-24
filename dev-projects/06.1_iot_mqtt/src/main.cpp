#include <Arduino.h>

#include "IoTBlynk.h"

IoTBlynk iot(IoTBlynk::defaultConfig());

void setup()
{
    Serial.begin(115200);

    iot.begin();
}

void loop()
{
    iot.update();

    iot.setSystemState(
        "RUNNING"
    );

    iot.setLastColor(
        "RED"
    );

    iot.setObjectCount(
        123
    );

    iot.setEmergency(
        false
    );
}