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

/**
 * @brief Main system state.
 */
enum class SystemState
{
    IDLE = 0,
    RUNNING,
    STOPPED,
    EMERGENCY
};

/**
 * @brief LCD page selection.
 */
enum class UIPage
{
    MAIN = 0,
    SENSOR,
    STATISTIC,
    DEBUG
};

/**
 * @brief Button event.
 */
enum class ButtonEvent
{
    NONE = 0,
    START,
    STOP,
    NEXT_PAGE
};

class UI
{
public:
    struct Config
    {
        uint8_t startButtonPin;

        uint8_t stopButtonPin;

        uint8_t nextButtonPin;

        uint8_t greenLedPin;

        uint8_t blueLedPin;

        uint8_t redLedPin;

        uint8_t lcdAddress;

        uint8_t lcdColumns;

        uint8_t lcdRows;

        uint32_t debounceMs;

        uint32_t refreshMs;
    };

    /**
     * @brief Create the default demo configuration.
     */
    static Config defaultConfig();

    struct DebugData
    {
        SystemState state;

        UIPage page;

        uint32_t objectCount;
    };

public:

    explicit UI(
        const Config& config
    );

    bool begin();

    void update();

    ButtonEvent getButtonEvent();

    void setSystemState(
        SystemState state
    );

    void setLastColor(
        const String& color
    );

    void setObjectCount(
        uint32_t count
    );

    void setPage(
        UIPage page
    );

    UIPage getPage() const;

    DebugData getDebugData() const;

    void printDebug(
        Stream& stream
    ) const;

private:

    void updateButtons();

    void updateLCD();

    void updateLEDs();

    const char* pageToString(
        UIPage page
    ) const;

    const char* stateToString(
        SystemState state
    ) const;

private:

    Config _config;

    LiquidCrystal_I2C _lcd;

    SystemState _state;

    UIPage _page;

    ButtonEvent _buttonEvent;

    String _lastColor;

    uint32_t _objectCount;

    uint32_t _lastRefreshTime;
};
