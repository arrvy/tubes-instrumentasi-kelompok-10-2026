/**
 * ============================================================
 * main.cpp
 * ============================================================
 * UI Library Dummy Test
 * Platform : ESP32
 * ============================================================
 */

#include <Arduino.h>

#include "UI.h"

UI ui(UI::defaultConfig());

/* ============================================================
 * Dummy Data
 * ============================================================
 */

uint32_t lastDummyUpdate = 0;

uint32_t objectCounter = 0;

uint8_t stateIndex = 0;

String dummyColors[] =
{
    "RED",
    "GREEN",
    "BLUE",
    "WHITE",
    "BLACK"
};

SystemState dummyStates[] =
{
    SystemState::IDLE,
    SystemState::RUNNING,
    SystemState::STOPPED,
    SystemState::EMERGENCY
};

/* ============================================================
 * Setup
 * ============================================================
 */

void setup()
{
    Serial.begin(115200);

    ui.begin();

    ui.setSystemState(
        SystemState::IDLE
    );

    ui.setLastColor(
        "NONE"
    );

    ui.setObjectCount(
        0
    );
}

/* ============================================================
 * Loop
 * ============================================================
 */

void loop()
{
    ui.update();

    /*
     * Update dummy data every 3 seconds
     */
    if(millis() - lastDummyUpdate >= 3000)
    {
        lastDummyUpdate = millis();

        objectCounter++;

        stateIndex++;

        if(stateIndex >= 4)
        {
            stateIndex = 0;
        }

        ui.setSystemState(
            dummyStates[stateIndex]
        );

        ui.setLastColor(
            dummyColors[
                objectCounter % 5
            ]
        );

        ui.setObjectCount(
            objectCounter
        );

        Serial.println(
            "Dummy data updated"
        );
    }

    /*
     * Read button event
     */
    ButtonEvent event = ui.getButtonEvent();

    switch(event)
    {
        case ButtonEvent::START:
        {
            Serial.println(
                "START BUTTON"
            );

            break;
        }

        case ButtonEvent::STOP:
        {
            Serial.println(
                "STOP BUTTON"
            );

            break;
        }

        case ButtonEvent::NEXT_PAGE:
        {
            Serial.println(
                "NEXT PAGE"
            );

            break;
        }

        default:
        {
            break;
        }
    }

    /*
     * Debug command from serial
     */
    if(Serial.available())
    {
        String cmd =
            Serial.readStringUntil('\n');

        cmd.trim();

        if(cmd == "debug")
        {
            ui.printDebug(
                Serial
            );
        }
    }
}