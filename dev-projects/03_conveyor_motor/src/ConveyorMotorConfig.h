/**
 * ============================================================
 * ConveyorMotorConfig.h
 * ============================================================
 * Conveyor Motor Configuration
 * Platform : ESP32
 * ============================================================
 */

#pragma once

#include <stdint.h>

namespace ConveyorMotorConfig
{
    constexpr uint32_t DEFAULT_PWM_FREQUENCY = 1000;

    constexpr uint8_t DEFAULT_PWM_RESOLUTION = 8;

    constexpr uint8_t DEFAULT_SPEED = 180;
}