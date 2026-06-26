/**
 * ============================================================
 * IoTBlynk.h
 * ============================================================
 * Blynk Telemetry Library
 * Platform : ESP32
 * ============================================================
 */

#pragma once

#include <Arduino.h>
#include "IoTBlynkConfig.h"
#include <WiFi.h>

/**
 * @brief IoT connection state.
 */
enum class IoTState
{
    DISCONNECTED = 0,
    CONNECTING,
    CONNECTED
};

class IoTBlynk
{
public:

    /**
     * @brief Configuration structure.
     */
    struct Config
    {
        const char* authToken;

        const char* ssid;

        const char* password;

        uint32_t uploadIntervalMs;
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
        IoTState state;

        bool wifiConnected;

        bool blynkConnected;

        uint32_t objectCount;

        bool emergency;

        float temperatureC;
    };

public:

    /**
     * @brief Constructor.
     *
     * @param config Library configuration.
     */
    explicit IoTBlynk(
        const Config& config
    );

    /**
     * @brief Initialize WiFi and Blynk.
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
     * @brief Set latest detected color.
     *
     * @param color Color string.
     */
    void setLastColor(
        const String& color
    );

    /**
     * @brief Set object counter.
     *
     * @param count Total objects.
     */
    void setObjectCount(
        uint32_t count
    );

    /**
     * @brief Set emergency state.
     *
     * @param state Emergency flag.
     */
    void setEmergency(
        bool state
    );

    /**
     * @brief Set system state text.
     *
     * Example:
     * IDLE
     * RUNNING
     * EMERGENCY
     *
     * @param state System state string.
     */
    void setSystemState(
        const String& state
    );

    void setTemperature(
        float temperatureC
    );

    /**
     * @brief Get connection state.
     *
     * @return IoTState
     */
    IoTState getState() const;

    /**
     * @brief Get debug snapshot.
     *
     * @return DebugData
     */
    DebugData getDebugData() const;

    /**
     * @brief Print debug information.
     *
     * @param stream Output stream.
     */
    void printDebug(
        Stream& stream
    ) const;

private:

    /**
     * @brief Upload telemetry to Blynk.
     */
    void sendTelemetry();

    /**
     * @brief Convert enum to string.
     */
    const char* stateToString(
        IoTState state
    ) const;

private:

    Config _config;

    IoTState _state;

    uint32_t _lastUploadTime;

    String _lastColor;

    String _systemState;

    uint32_t _objectCount;

    bool _emergency;

    float _temperatureC;
};
