/**
 * ============================================================
 * ServoSorter.h
 * ============================================================
 * Single Servo Color Sorting Library
 * Platform : ESP32
 * ============================================================
 */

#pragma once

#include <Arduino.h>
#include "ServoSorterConfig.h"
#include <ESP32Servo.h>

class ServoSorter
{
public:

    enum class ColorClass
    {
        UNKNOWN = 0,
        RED,
        GREEN,
        BLUE,
        WHITE,
        BLACK
    };

    enum class State
    {
        IDLE = 0,
        WAIT_DELAY,
        MOVE_TO_TARGET,
        HOLD_POSITION,
        RETURN_HOME,
        EMERGENCY
    };

    struct Config
    {
        uint8_t servoPin;

        int homeAngle;

        int redAngle;
        int greenAngle;
        int blueAngle;
        int whiteAngle;
        int blackAngle;

        uint32_t sortDelayMs;

        uint32_t holdTimeMs;
    };

    struct DebugData
    {
        State state;

        ColorClass activeColor;

        uint8_t queueCount;

        int targetAngle;
    };

public:

    explicit ServoSorter(const Config& config);

    bool begin();

    void update();

    bool triggerSort(ColorClass color);

    void emergencyStop();

    void resetEmergency();

    bool isBusy() const;

    State getState() const;

    ColorClass getCurrentColor() const;

    DebugData getDebugData() const;

    void printDebug(Stream& stream) const;

private:

    struct SortJob
    {
        ColorClass color;
    };

private:

    bool enqueue(ColorClass color);

    bool dequeue(SortJob& job);

    int getTargetAngle(ColorClass color) const;

    void processStateMachine();

    const char* stateToString(State state) const;

    const char* colorToString(ColorClass color) const;

private:

    Config _config;

    Servo _servo;

    State _state;

    ColorClass _activeColor;

    int _targetAngle;

    uint32_t _stateTimestamp;

    SortJob _queue[ServoSorterConfig::MAX_QUEUE_SIZE];

    uint8_t _head;

    uint8_t _tail;

    uint8_t _count;
};