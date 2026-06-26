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
#include <DHT.h>
#include "EmergencyStopConfig.h"

/**
 * @brief Safety state machine.
 */
enum class EmergencyState
{
    SAFE = 0,
    EMERGENCY_TEMP,
    EMERGENCY_BUTTON,
    EMERGENCY_FEEDBACK,
    EMERGENCY_SENSOR
};

class EmergencyStop
{
public:

    /**
     * @brief Library configuration.
     */
    struct Config
    {
        uint8_t dhtPin;

        uint8_t estopPin;

        uint8_t feedbackPin;

        uint8_t relayPin;

        float temperatureThresholdC;

        uint32_t readIntervalMs;

        bool estopActiveLow;

        bool feedbackActiveLow;

        bool relayActiveHigh;
    };

    /**
     * @brief Create the default demo configuration.
     */
    static Config defaultConfig();

    /**
     * @brief Debug snapshot.
     */
    struct DebugData
    {
        EmergencyState state;

        float temperatureC;

        bool estopPressed;

        bool feedbackLost;

        bool temperatureValid;
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
     * @brief Read latest DHT11 temperature value.
     *
     * @return uint16_t
     */
    float getTemperatureC() const;

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

    void applyRelayOutput();

    /**
     * @brief Convert enum to string.
     */
    const char* stateToString(
        EmergencyState state
    ) const;

private:

    Config _config;

    EmergencyState _state;

    float _temperatureC;

    bool _temperatureValid;

    bool _estopPressed;

    bool _feedbackLost;

    uint32_t _lastTemperatureReadTime;

    DHT _dht;
};
