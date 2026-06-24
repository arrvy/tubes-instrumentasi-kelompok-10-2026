#include <Arduino.h>

#include "IRTrigger.h"

IRTrigger irTrigger(IRTrigger::defaultConfig());

uint32_t lastDebugMs = 0;
const uint8_t ledActivePin = IRTriggerConfig::DEFAULT_LED_ACTIVE_PIN;
const uint8_t ledIdlePin = IRTriggerConfig::DEFAULT_LED_IDLE_PIN;

void updateIndicators()
{
    const bool objectPresent = irTrigger.isObjectPresent();

    digitalWrite(ledActivePin, objectPresent ? HIGH : LOW);
    digitalWrite(ledIdlePin, objectPresent ? LOW : HIGH);
}

void printShortStatus()
{
    const IRTrigger::DebugData data = irTrigger.getDebugData();

    Serial.print(F("[STATUS] state="));
    Serial.print(irTrigger.stateToString(data.state));
    Serial.print(F(", event="));
    Serial.print(irTrigger.eventToString(data.lastEvent));
    Serial.print(F(", object="));
    Serial.print(data.objectPresent ? F("PRESENT") : F("CLEAR"));
    Serial.print(F(", count="));
    Serial.println(data.totalCount);
}

void setup()
{
    Serial.begin(115200);

    pinMode(ledActivePin, OUTPUT);
    pinMode(ledIdlePin, OUTPUT);

    irTrigger.begin();
    updateIndicators();
}

void loop()
{
    irTrigger.update();

    if (irTrigger.readTrigger())
    {
        Serial.print(F("[IR] object detected, total="));
        Serial.println(irTrigger.getTotalCount());

        printShortStatus();
    }

    if (irTrigger.readRelease())
    {
        Serial.println(F("[IR] object released"));
    }

    updateIndicators();

    if ((millis() - lastDebugMs) >= 1000U)
    {
        lastDebugMs = millis();
        irTrigger.printDebug(Serial);
    }
}
