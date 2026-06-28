/**
 * ============================================================
 * IRTrigger.h
 * ============================================================
 * IR Object Detection Trigger and Counter Module
 * Platform : ESP32 / PlatformIO / Arduino
 * ============================================================
 */

#pragma once

#include <Arduino.h>
#include "IRTriggerConfig.h"

class IRTrigger final
{
public:
    enum class State : uint8_t
    {
        WAIT_OBJECT,
        DEBOUNCE_TRIGGER,
        OBJECT_PRESENT,
        DEBOUNCE_CLEAR
    };

    enum class Event : uint8_t
    {
        NONE,
        TRIGGERED,
        RELEASED
    };

    struct Config
    {
        uint8_t sensorPin;
        bool activeLow;
        bool useInternalPullup;
        uint32_t triggerDebounceMs;
        uint32_t clearDebounceMs;
        uint32_t sampleIntervalMs;
        uint32_t initialCounter;
    };

    /**
     * @brief Create the default demo configuration.
     */
    static Config defaultConfig();

    struct DebugData
    {
        State state;
        Event lastEvent;
        bool objectPresent;
        uint32_t totalCount;
        uint32_t lastTriggerTimestamp;
        uint32_t lastReleaseTimestamp;
    };

public:
    explicit IRTrigger(const Config& config);
    bool begin();
    void update();
    bool readTrigger();
    bool readRelease();
    void resetCounter(uint32_t value = 0);
    State getState() const;
    bool isObjectPresent() const;
    uint32_t getTotalCount() const;
    DebugData getDebugData() const;
    const char* stateToString(State state) const;
    const char* eventToString(Event event) const;
    void printDebug(Stream& stream) const;

private:
    bool readSensorActive() const;
    void setState(State newState);
    void registerTrigger(uint32_t now);
    void registerRelease(uint32_t now);
    void clearLatchedEvents();

private:
    Config _config;
    State _state;
    Event _lastEvent;
    bool _objectPresent;
    bool _triggerPending;
    bool _releasePending;
    uint32_t _previousMillis;
    uint32_t _stateStartMillis;
    uint32_t _lastTriggerTimestamp;
    uint32_t _lastReleaseTimestamp;
    uint32_t _totalCount;
};
