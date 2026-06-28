/**
 * ============================================================
 * ConveyorMotor.h
 * ============================================================
 * Conveyor Motor Library
 * Platform : ESP32
 * ============================================================
 */

#pragma once

#include <Arduino.h>
#include "ConveyorMotorConfig.h"

/**
 * @brief Conveyor operating state.
 */
enum class ConveyorState
{
    STOPPED = 0,
    RUNNING,
    EMERGENCY
};

class ConveyorMotor
{
public:

    /**
     * @brief Configuration structure.
     */
    struct Config
    {
        uint8_t enPin;              /**< PWM pin */
        uint8_t in1Pin;             /**< L298N IN1 */
        uint8_t in2Pin;             /**< L298N IN2 */

        uint32_t pwmFrequency;      /**< PWM Frequency */

        uint8_t pwmResolution;      /**< PWM Resolution */

        uint8_t defaultSpeed;       /**< Default speed */
    };

    /**
     * @brief Create the default demo configuration.
     */
    static Config defaultConfig();

    /**
     * @brief Debug data snapshot.
     */
    struct DebugData
    {
        ConveyorState state;

        uint8_t speed;
    };

public:

    /**
     * @brief Constructor.
     *
     * @param config Configuration structure.
     */
    explicit ConveyorMotor(const Config& config);

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
     * @brief Start conveyor.
     */
    void start();

    /**
     * @brief Stop conveyor.
     */
    void stop();

    /**
     * @brief Emergency stop.
     */
    void emergencyStop();

    /**
     * @brief Clear emergency state.
     */
    void resetEmergency();

    /**
     * @brief Set conveyor speed.
     *
     * @param speed PWM value (0-255)
     */
    void setSpeed(uint8_t speed);

    /**
     * @brief Get current speed.
     *
     * @return uint8_t
     */
    uint8_t getSpeed() const;

    /**
     * @brief Get current state.
     *
     * @return ConveyorState
     */
    ConveyorState getState() const;

    /**
     * @brief Check running state.
     *
     * @return true Conveyor running.
     * @return false Conveyor stopped.
     */
    bool isRunning() const;

    /**
     * @brief Get debug snapshot.
     *
     * @return DebugData
     */
    DebugData getDebugData() const;

    /**
     * @brief Print debug information.
     *
     * Library does not know Serial.
     *
     * Example:
     * conveyor.printDebug(Serial);
     *
     * @param stream Output stream.
     */
    void printDebug(Stream& stream) const;

private:

    /**
     * @brief Apply output according to state.
     */
    void applyOutput();

    /**
     * @brief Convert state to string.
     */
    const char* stateToString(ConveyorState state) const;

private:

    Config _config;

    ConveyorState _state;

    uint8_t _speed;
};
