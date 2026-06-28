/**
 * ============================================================
 * UI.cpp
 * ============================================================
 * LCD + LED + Button Library
 * Platform : ESP32
 * ============================================================
 */

#include "UI.h"
#include <Wire.h>

UI::UI(const Config& config)
    : _config(config),
      _lcd(config.lcdAddress, config.lcdColumns, config.lcdRows),
            _stateText("IDLE"),
      _lastColor("NONE"),
            _temperatureC(NAN),
      _objectCount(0),
      _lastRefreshTime(0)
{
}

UI::Config UI::defaultConfig()
{
    Config config;

    config.lcdAddress = 0x27;
    config.lcdColumns = 16;
    config.lcdRows = 2;
    config.refreshMs = UIConfig::DEFAULT_LCD_REFRESH_MS;

    return config;
}

bool UI::begin()
{
    Wire.begin(21, 22);

    _lcd.init();
    _lcd.backlight();
    _lcd.setCursor(0, 0);
    _lcd.print("System Init...");

    delay(500);
    _lcd.clear();

    return true;
}

void UI::update()
{
    updateLCD();
}

void UI::setSystemState(SystemState state)
{
    _stateText = stateToString(state);
}

void UI::setSystemState(const String& stateText)
{
    _stateText = stateText;
}

void UI::setLastColor(const String& color)
{
    _lastColor = color;
}

void UI::setTemperature(float temperatureC)
{
    _temperatureC = temperatureC;
}

void UI::setObjectCount(uint32_t count)
{
    _objectCount = count;
}

UI::DebugData UI::getDebugData() const
{
    DebugData data;
    data.stateText = _stateText;
    data.lastColor = _lastColor;
    data.temperatureC = _temperatureC;
    data.objectCount = _objectCount;
    return data;
}

void UI::printDebug(Stream& stream) const
{
    stream.println(F("===== UI Debug ====="));
    stream.print(F("State    : "));
    stream.println(_stateText);
    stream.print(F("Color    : "));
    stream.println(_lastColor);
    stream.print(F("Temp C   : "));
    stream.println(_temperatureC);
    stream.print(F("Count    : "));
    stream.println(_objectCount);
    stream.println(F("===================="));
}

void UI::updateLCD()
{
    uint32_t now = millis();

    if (now - _lastRefreshTime < _config.refreshMs)
    {
        return; // Not time to update yet
    }

    _lastRefreshTime = now;

    _lcd.clear();

    _lcd.setCursor(0, 0);
    _lcd.print(F("S:"));
    _lcd.print(_stateText);
    _lcd.print(F(" T:"));
    _lcd.print(_temperatureC, 1);
    _lcd.print((char)223);
    _lcd.print(F("C"));

    _lcd.setCursor(0, 1);
    _lcd.print(F("C:"));
    _lcd.print(_lastColor);
    _lcd.print(F(" N:"));
    _lcd.print(_objectCount);
}
