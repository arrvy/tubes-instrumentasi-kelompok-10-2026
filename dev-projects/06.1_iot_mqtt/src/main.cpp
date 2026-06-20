#include <Arduino.h>

#include "IoTBlynk.h"

IoTBlynk::Config iotConfig
{
    .authToken = "TOKEN",

    .ssid = "WIFI_NAME",

    .password = "WIFI_PASS",

    .uploadIntervalMs = 1000
};

IoTBlynk iot(
    iotConfig
);

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