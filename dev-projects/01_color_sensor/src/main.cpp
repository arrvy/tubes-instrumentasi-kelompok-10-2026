#include <Arduino.h>
#include "ColorSensor.h"

ColorSensor::Config colorConfig =
{
    .pinLedR = 25,
    .pinLedG = 26,
    .pinLedB = 27,

    .pinPhotodiode = 34,

    .sampleDelayMs = 50
};

ColorSensor colorSensor(colorConfig);

void setup()
{
    Serial.begin(115200);

    colorSensor.begin();
}

void loop()
{
    colorSensor.update();

    if (colorSensor.readColor())
    {
        Serial.print("Detected : ");

        Serial.println(
            colorSensor.colorToString(
                colorSensor.getColor()));
    }
}