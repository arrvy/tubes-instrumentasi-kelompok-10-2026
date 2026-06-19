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
    constexpr uint16_t DEFAULT_MQ2_THRESHOLD = 500;

    constexpr uint32_t DEFAULT_DEBOUNCE_MS = 50;
}