#include <Arduino.h>
#include "ColorSensor.h"

ColorSensor colorSensor(ColorSensor::defaultConfig());

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