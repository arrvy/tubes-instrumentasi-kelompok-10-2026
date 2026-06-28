/**
 * ============================================================
 * EmergencyStopConfig.h
 * ============================================================
 * Emergency Stop Configuration
 * Platform : ESP32
 * ============================================================
 */

#pragma once

#include <stdint.h>

namespace EmergencyStopConfig
{
    constexpr float DEFAULT_TEMPERATURE_THRESHOLD_C = 40.0f;

    constexpr uint32_t DEFAULT_READ_INTERVAL_MS = 1000;
}