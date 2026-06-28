/**
 * ============================================================
 * UI.h
 * ============================================================
 * LCD + LED + Button Library
 * Platform : ESP32
 * ============================================================
 */

#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "UIConfig.h"

class UI
{
public:
    struct Config
    {
        uint8_t lcdAddress;

        uint8_t lcdColumns;

        uint8_t lcdRows;

        uint32_t refreshMs;
    };

    /**
     * @brief Create the default demo configuration.
     */
    static Config defaultConfig();

    struct DebugData
    {
        String stateText;

        String lastColor;

        float temperatureC;

        uint32_t objectCount;
    };

public:

    explicit UI(
        const Config& config
    );

    bool begin();

    void update();

    void setSystemState(
        const String& stateText
    );

    void setLastColor(
        const String& color
    );

    void setTemperature(
        float temperatureC
    );

    void setObjectCount(
        uint32_t count
    );

    DebugData getDebugData() const;

    void printDebug(
        Stream& stream
    ) const;

private:

    void updateLCD();

private:

    Config _config;

    LiquidCrystal_I2C _lcd;

    String _stateText;

    String _lastColor;

    float _temperatureC;

    uint32_t _objectCount;

    uint32_t _lastRefreshTime;
};
