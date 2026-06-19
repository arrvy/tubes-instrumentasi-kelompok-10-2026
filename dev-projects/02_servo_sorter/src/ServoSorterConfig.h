 /**
 * ============================================================
 * ServoSorterConfig.h
 * ============================================================
 * Global Configuration
 * Servo Sorter Library
 * Platform : ESP32
 * ============================================================
 */

#pragma once

#include <stdint.h>

namespace ServoSorterConfig
{
    constexpr uint8_t MAX_QUEUE_SIZE = 10;

    constexpr uint32_t DEFAULT_SORT_DELAY_MS = 1500;

    constexpr uint32_t DEFAULT_HOLD_TIME_MS = 500;

    constexpr int DEFAULT_HOME_ANGLE = 90;

    constexpr int DEFAULT_RED_ANGLE   = 30;
    constexpr int DEFAULT_GREEN_ANGLE = 60;
    constexpr int DEFAULT_BLUE_ANGLE  = 120;
    constexpr int DEFAULT_WHITE_ANGLE = 150;
    constexpr int DEFAULT_BLACK_ANGLE = 180;
}