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
     /**
     * @brief Supported color categories.
     *
     * Color classification result received from
     * ColorSensor module.
     */
    enum class ColorClass
    {
        UNKNOWN = 0,  //Unknown Color
        RED,
        GREEN,
        BLUE,
        WHITE,
        BLACK
    };

    /**
     * @brief Internal servo state machine.
     *
     * IDLE
     *   -> waiting for job
     *
     * WAIT_DELAY
     *   -> waiting conveyor travel time
     *
     * MOVE_TO_TARGET
     *   -> move servo to sorting angle
     *
     * HOLD_POSITION
     *   -> keep gate active
     *
     * RETURN_HOME
     *   -> return servo to home angle
     *
     * EMERGENCY
     *   -> emergency stop state
     */
    enum class State
    {
        IDLE = 0,
        WAIT_DELAY,
        MOVE_TO_TARGET,
        HOLD_POSITION,
        RETURN_HOME,
        EMERGENCY
    };

    /**
     * @brief ServoSorter configuration.
     *
     * All runtime parameters shall be configured
     * through this structure.
     */
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
    /**
     * @brief Construct ServoSorter object.
     *
     * @param config Library configuration structure.
     */
    explicit ServoSorter(const Config& config);

    /**
     * @brief Initialize hardware resources.
     *
     * Must be called once inside setup().
     *
     * @return true Initialization success.
     * @return false Initialization failed.
     */
    bool begin();

    /**
     * @brief Execute state machine.
     *
     * Non-blocking.
     * Must be called continuously from loop().
     */
    void update();

    /**
     * @brief Add new sorting request.
     *
     * Request is stored inside internal queue.
     *
     * @param color Target color class.
     *
     * @return true Queue accepted request.
     * @return false Queue full or emergency state.
     */
    bool triggerSort(ColorClass color);

    /**
     * @brief Enter emergency state.
     *
     * Servo immediately returns to home position.
     * Queue will be cleared.
     */
    void emergencyStop();


    /**
     * @brief Clear emergency state.
     *
     * Library returns to IDLE mode.
     */
    void resetEmergency();

    /**
     * @brief Check if sorter is processing jobs.
     *
     * @return true Sorter busy.
     * @return false Sorter idle.
     */
    bool isBusy() const;

    /**
     * @brief Get current state machine state.
     *
     * @return Current state.
     */
    State getState() const;

    /**
     * @brief Get active sorting color.
     *
     * @return Current color job.
     */
    ColorClass getCurrentColor() const;

    /**
     * @brief Get debug snapshot.
     *
     * Can be used by:
     * - Serial Console
     * - LCD UI
     * - MQTT
     * - Blynk
     * - NodeRed
     *
     * @return Debug data structure.
     */
    DebugData getDebugData() const;
        /**
         * @brief Create the default demo configuration.
         */
        static Config defaultConfig();


    /**
     * @brief Print debug information.
     *
     * Library does not know Serial.
     * Caller supplies output stream.
     *
     * Example:
     *
     * servo.printDebug(Serial);
     *
     * @param stream Output stream.
     */
    void printDebug(Stream& stream) const;

private:

    /**
    * @brief Sorting job entry.
    *
    * Setiap objek yang berhasil diklasifikasikan
    * akan dimasukkan ke dalam queue sebagai
    * sebuah sorting request.
    */
    struct SortJob
    {
        ColorClass color;
    };

private:
    /**
    * @brief Add new sorting request into queue.
    *
    * Menggunakan circular FIFO queue.
    *
    * @param color Target color category.
    *
    * @return true Queue accepted request.
    * @return false Queue full.
    */
    bool enqueue(ColorClass color);


    /**
    * @brief Retrieve oldest sorting request.
    *
    * @param job Output job container.
    *
    * @return true Job available.
    * @return false Queue empty.
    */
    bool dequeue(SortJob& job);

    /**
    * @brief Convert color class into servo angle.
    *
    * Mapping:
    * RED   -> redAngle
    * GREEN -> greenAngle
    * BLUE  -> blueAngle
    * WHITE -> whiteAngle
    * BLACK -> blackAngle
    *
    * @param color Target color.
    *
    * @return Servo target angle.
    */
    int getTargetAngle(ColorClass color) const;

    /**
     * ============================================================
     * STATE MACHINE
     * ============================================================
     *
     * IDLE
     *  |
     *  +--> WAIT_DELAY
     *          |
     *          +--> MOVE_TO_TARGET
     *                     |
     *                     +--> HOLD_POSITION
     *                                 |
     *                                 +--> RETURN_HOME
     *                                               |
     *                                               +--> IDLE
     *
     * Emergency can be entered from any state.
     *
     * ============================================================
     */
    void processStateMachine();


    /**
    * @brief Convert State enum into string.
    *
    * Digunakan untuk debug output.
    *
    * @param state Current state.
    *
    * @return Readable state string.
    */
    const char* stateToString(State state) const;

    /**
    * @brief Convert ColorClass enum into string.
    *
    * Digunakan untuk debug output.
    *
    * @param color Current color.
    *
    * @return Readable color string.
    */
    const char* colorToString(ColorClass color) const;

private:

    /**
    * @brief User supplied configuration.
    */
    Config _config;
    /**
    * @brief Internal ESP32 servo object.
    */
    Servo _servo;
    /**
    * @brief Current state machine state.
    */
    State _state;
    /**
    * @brief Currently processed color.
    *
    * UNKNOWN ketika tidak ada job aktif.
    */
    ColorClass _activeColor;

    /**
    * @brief Servo angle currently requested
    * by active sorting job.
    */
    int _targetAngle;

    /**
    * @brief Timestamp marker for millis()-based timing.
    *
    * Digunakan pada:
    * - WAIT_DELAY
    * - HOLD_POSITION
    */
    uint32_t _stateTimestamp;

    /**
     * Circular FIFO queue.
     *
     * Stores sorting requests while
     * previous objects are still travelling
     * on conveyor.
     */
    SortJob _queue[ServoSorterConfig::MAX_QUEUE_SIZE];

    /**
    * @brief Queue read index.
    */
    uint8_t _head;

    /**
    * @brief Queue write index.
    */
    uint8_t _tail;

    /**
    * @brief Number of active jobs stored in queue.
    */
    uint8_t _count;
};