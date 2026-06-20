/**
 * ============================================================
 * UIConfig.h
 * ============================================================
 * User Interface Configuration
 * Platform : ESP32
 * ============================================================
 */

#pragma once

#include <stdint.h>

namespace UIConfig
{
    constexpr uint32_t DEFAULT_BUTTON_DEBOUNCE_MS = 50;

    constexpr uint32_t DEFAULT_LCD_REFRESH_MS = 250;
}