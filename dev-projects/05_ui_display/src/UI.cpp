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
      _state(SystemState::IDLE),
      _page(UIPage::MAIN),
      _buttonEvent(ButtonEvent::NONE),
      _lastColor("NONE"),
      _objectCount(0),
      _lastRefreshTime(0)
{
}

UI::Config UI::defaultConfig()
{
    Config config;

    config.startButtonPin = 255;
    config.stopButtonPin = 255;
    config.nextButtonPin = 255;
    config.greenLedPin = 255;
    config.blueLedPin = 255;
    config.redLedPin = 255;
    config.lcdAddress = 0x27;
    config.lcdColumns = 16;
    config.lcdRows = 2;
    config.debounceMs = UIConfig::DEFAULT_BUTTON_DEBOUNCE_MS;
    config.refreshMs = UIConfig::DEFAULT_LCD_REFRESH_MS;

    return config;
}

bool UI::begin()
{
    // Initialize Wire (I2C) with standard ESP32 pins
    Wire.begin(21, 22);

    // Initialize LCD
    _lcd.init();
    _lcd.backlight();
    _lcd.setCursor(0, 0);
    _lcd.print("System Init...");

    // Initialize button pins
    if (_config.startButtonPin != 255) pinMode(_config.startButtonPin, INPUT_PULLUP);
    if (_config.stopButtonPin != 255) pinMode(_config.stopButtonPin, INPUT_PULLUP);
    if (_config.nextButtonPin != 255) pinMode(_config.nextButtonPin, INPUT_PULLUP);

    // Status LEDs are optional. By default pins 15/2/4 are reserved
    // for the color sensor RGB LED, so UI does not drive them.
    if (_config.greenLedPin != 255) pinMode(_config.greenLedPin, OUTPUT);
    if (_config.blueLedPin != 255) pinMode(_config.blueLedPin, OUTPUT);
    if (_config.redLedPin != 255) pinMode(_config.redLedPin, OUTPUT);

    // Turn off all LEDs initially
    if (_config.greenLedPin != 255) digitalWrite(_config.greenLedPin, LOW);
    if (_config.blueLedPin != 255) digitalWrite(_config.blueLedPin, LOW);
    if (_config.redLedPin != 255) digitalWrite(_config.redLedPin, LOW);

    delay(500);
    _lcd.clear();

    return true;
}

void UI::update()
{
    updateButtons();
    updateLEDs();
    updateLCD();
}

ButtonEvent UI::getButtonEvent()
{
    ButtonEvent event = _buttonEvent;
    _buttonEvent = ButtonEvent::NONE;
    return event;
}

void UI::setSystemState(SystemState state)
{
    _state = state;
}

void UI::setLastColor(const String& color)
{
    _lastColor = color;
}

void UI::setObjectCount(uint32_t count)
{
    _objectCount = count;
}

void UI::setPage(UIPage page)
{
    _page = page;
}

UIPage UI::getPage() const
{
    return _page;
}

UI::DebugData UI::getDebugData() const
{
    DebugData data;
    data.state = _state;
    data.page = _page;
    data.objectCount = _objectCount;
    return data;
}

void UI::printDebug(Stream& stream) const
{
    stream.println(F("===== UI Debug ====="));
    stream.print(F("State    : "));
    stream.println(stateToString(_state));
    stream.print(F("Page     : "));
    stream.println(pageToString(_page));
    stream.print(F("Color    : "));
    stream.println(_lastColor);
    stream.print(F("Count    : "));
    stream.println(_objectCount);
    stream.println(F("===================="));
}

void UI::updateButtons()
{
    static uint32_t startDebounceTime = 0;
    static uint32_t stopDebounceTime = 0;
    static uint32_t nextDebounceTime = 0;

    uint32_t now = millis();

    // START button
    if ((_config.startButtonPin != 255) && (digitalRead(_config.startButtonPin) == LOW))
    {
        if (now - startDebounceTime > _config.debounceMs)
        {
            _buttonEvent = ButtonEvent::START;
            startDebounceTime = now;
        }
    }

    // STOP button
    if ((_config.stopButtonPin != 255) && (digitalRead(_config.stopButtonPin) == LOW))
    {
        if (now - stopDebounceTime > _config.debounceMs)
        {
            _buttonEvent = ButtonEvent::STOP;
            stopDebounceTime = now;
        }
    }

    // NEXT PAGE button
    if ((_config.nextButtonPin != 255) && (digitalRead(_config.nextButtonPin) == LOW))
    {
        if (now - nextDebounceTime > _config.debounceMs)
        {
            _buttonEvent = ButtonEvent::NEXT_PAGE;
            // Auto-cycle page
            _page = static_cast<UIPage>((static_cast<int>(_page) + 1) % 4);
            nextDebounceTime = now;
        }
    }
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

    switch (_page)
    {
        case UIPage::MAIN:
        {
            _lcd.setCursor(0, 0);
            _lcd.print(F("State: "));
            _lcd.print(stateToString(_state));

            _lcd.setCursor(0, 1);
            _lcd.print(F("Color: "));
            _lcd.print(_lastColor);
            break;
        }

        case UIPage::SENSOR:
        {
            _lcd.setCursor(0, 0);
            _lcd.print(F("Sensor"));

            _lcd.setCursor(0, 1);
            _lcd.print(F("Color: "));
            _lcd.print(_lastColor);
            break;
        }

        case UIPage::STATISTIC:
        {
            _lcd.setCursor(0, 0);
            _lcd.print(F("Count: "));
            _lcd.print(_objectCount);

            _lcd.setCursor(0, 1);
            _lcd.print(F("State: "));
            _lcd.print(stateToString(_state));
            break;
        }

        case UIPage::DEBUG:
        {
            _lcd.setCursor(0, 0);
            _lcd.print(F("Debug Mode"));

            _lcd.setCursor(0, 1);
            _lcd.print(F("Obj: "));
            _lcd.print(_objectCount);
            break;
        }

        default:
        {
            break;
        }
    }
}

void UI::updateLEDs()
{
    // Turn off all LEDs first
    if (_config.greenLedPin != 255) digitalWrite(_config.greenLedPin, LOW);
    if (_config.blueLedPin != 255) digitalWrite(_config.blueLedPin, LOW);
    if (_config.redLedPin != 255) digitalWrite(_config.redLedPin, LOW);

    // Set LEDs based on state
    switch (_state)
    {
        case SystemState::IDLE:
        {
            if (_config.greenLedPin != 255) digitalWrite(_config.greenLedPin, HIGH);
            break;
        }

        case SystemState::RUNNING:
        {
            if (_config.blueLedPin != 255) digitalWrite(_config.blueLedPin, HIGH);
            break;
        }

        case SystemState::STOPPED:
        {
            if (_config.greenLedPin != 255) digitalWrite(_config.greenLedPin, HIGH);
            break;
        }

        case SystemState::EMERGENCY:
        {
            if (_config.redLedPin != 255) digitalWrite(_config.redLedPin, HIGH);
            break;
        }

        default:
        {
            break;
        }
    }
}

const char* UI::pageToString(UIPage page) const
{
    switch (page)
    {
        case UIPage::MAIN:
            return "MAIN";

        case UIPage::SENSOR:
            return "SENSOR";

        case UIPage::STATISTIC:
            return "STATISTIC";

        case UIPage::DEBUG:
            return "DEBUG";

        default:
            return "UNKNOWN";
    }
}

const char* UI::stateToString(SystemState state) const
{
    switch (state)
    {
        case SystemState::IDLE:
            return "IDLE";

        case SystemState::RUNNING:
            return "RUNNING";

        case SystemState::STOPPED:
            return "STOPPED";

        case SystemState::EMERGENCY:
            return "EMERGENCY";

        default:
            return "UNKNOWN";
    }
}
