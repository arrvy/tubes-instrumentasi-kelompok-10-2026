/**
 * ============================================================
 * IRTriggerConfig.h
 * ============================================================
 * IR Trigger Configuration
 * Platform : ESP32
 * ============================================================
 */

#pragma once

#include <stdint.h>

namespace IRTriggerConfig
{
    constexpr uint8_t DEFAULT_SENSOR_PIN = 35;
    constexpr bool DEFAULT_ACTIVE_LOW = true;
    constexpr bool DEFAULT_USE_INTERNAL_PULLUP = true;
    constexpr uint32_t DEFAULT_TRIGGER_DEBOUNCE_MS = 40;
    constexpr uint32_t DEFAULT_CLEAR_DEBOUNCE_MS = 50;
    constexpr uint32_t DEFAULT_SAMPLE_INTERVAL_MS = 5;
    constexpr uint32_t DEFAULT_INITIAL_COUNTER = 0;

    constexpr uint8_t DEFAULT_LED_ACTIVE_PIN = 2;
    constexpr uint8_t DEFAULT_LED_IDLE_PIN = 15;
}
