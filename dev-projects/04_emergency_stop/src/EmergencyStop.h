/**
 * ============================================================
 * EmergencyStop.h
 * ============================================================
 * Emergency Stop Library
 * Platform : ESP32
 * ============================================================
 */

#pragma once

#include <Arduino.h>
#include "EmergencyStopConfig.h"

/**
 * @brief Safety state machine.
 */
enum class EmergencyState
{
    SAFE = 0,
    EMERGENCY_MQ2,
    EMERGENCY_BUTTON,
    EMERGENCY_FEEDBACK
};

class EmergencyStop
{
public:

    /**
     * @brief Library configuration.
     */
    struct Config
    {
        uint8_t mq2Pin;

        uint8_t estopPin;

        uint8_t feedbackPin;

        uint16_t mq2Threshold;

        uint32_t debounceMs;

        bool estopActiveLow;

        bool feedbackActiveLow;
    };

    /**
     * @brief Debug snapshot.
     */
    struct DebugData
    {
        EmergencyState state;

        uint16_t mq2Value;

        bool estopPressed;

        bool feedbackLost;
    };

public:

    /**
     * @brief Constructor.
     *
     * @param config Configuration structure.
     */
    explicit EmergencyStop(
        const Config& config
    );

    /**
     * @brief Initialize hardware.
     *
     * @return true Success.
     * @return false Failed.
     */
    bool begin();

    /**
     * @brief Non-blocking update.
     *
     * Must be called continuously.
     */
    void update();

    /**
     * @brief Reset emergency state.
     *
     * Only works when condition
     * is already safe.
     */
    void resetEmergency();

    /**
     * @brief Check emergency state.
     *
     * @return true Emergency active.
     * @return false Safe.
     */
    bool isEmergency() const;

    /**
     * @brief Get state machine state.
     *
     * @return EmergencyState
     */
    EmergencyState getState() const;

    /**
     * @brief Read MQ2 value.
     *
     * @return uint16_t
     */
    uint16_t getMQ2Value() const;

    /**
     * @brief Get debug snapshot.
     *
     * @return DebugData
     */
    DebugData getDebugData() const;

    /**
     * @brief Print debug information.
     *
     * Example:
     * safety.printDebug(Serial);
     *
     * @param stream Output stream.
     */
    void printDebug(
        Stream& stream
    ) const;

private:

    /**
     * @brief Update safety logic.
     */
    void processSafety();

    /**
     * @brief Convert enum to string.
     */
    const char* stateToString(
        EmergencyState state
    ) const;

private:

    Config _config;

    EmergencyState _state;

    uint16_t _mq2Value;

    bool _estopPressed;

    bool _feedbackLost;

    uint32_t _lastDebounceTime;
};