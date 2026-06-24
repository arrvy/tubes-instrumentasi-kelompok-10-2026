/**
 * ============================================================
 * IRTrigger.cpp
 * ============================================================
 */

#include "IRTrigger.h"

IRTrigger::IRTrigger(const Config& config)
    : _config(config),
      _state(State::WAIT_OBJECT),
      _lastEvent(Event::NONE),
      _objectPresent(false),
      _triggerPending(false),
      _releasePending(false),
      _previousMillis(0),
      _stateStartMillis(0),
      _lastTriggerTimestamp(0),
      _lastReleaseTimestamp(0),
      _totalCount(config.initialCounter)
{
}

IRTrigger::Config IRTrigger::defaultConfig()
{
    Config config;

    config.sensorPin = IRTriggerConfig::DEFAULT_SENSOR_PIN;
    config.activeLow = IRTriggerConfig::DEFAULT_ACTIVE_LOW;
    config.useInternalPullup = IRTriggerConfig::DEFAULT_USE_INTERNAL_PULLUP;
    config.triggerDebounceMs = IRTriggerConfig::DEFAULT_TRIGGER_DEBOUNCE_MS;
    config.clearDebounceMs = IRTriggerConfig::DEFAULT_CLEAR_DEBOUNCE_MS;
    config.sampleIntervalMs = IRTriggerConfig::DEFAULT_SAMPLE_INTERVAL_MS;
    config.initialCounter = IRTriggerConfig::DEFAULT_INITIAL_COUNTER;

    return config;
}

bool IRTrigger::begin()
{
    pinMode(_config.sensorPin, _config.useInternalPullup ? INPUT_PULLUP : INPUT);
    resetCounter(_config.initialCounter);
    return true;
}

void IRTrigger::update()
{
    const uint32_t now = millis();

    if ((_config.sampleIntervalMs > 0U) && ((now - _previousMillis) < _config.sampleIntervalMs))
    {
        return;
    }

    _previousMillis = now;

    const bool active = readSensorActive();

    switch (_state)
    {
        case State::WAIT_OBJECT:
            if (active)
            {
                setState(State::DEBOUNCE_TRIGGER);
            }
            break;

        case State::DEBOUNCE_TRIGGER:
            if (!active)
            {
                setState(State::WAIT_OBJECT);
                break;
            }

            if ((now - _stateStartMillis) >= _config.triggerDebounceMs)
            {
                registerTrigger(now);
                setState(State::OBJECT_PRESENT);
            }
            break;

        case State::OBJECT_PRESENT:
            if (!active)
            {
                setState(State::DEBOUNCE_CLEAR);
            }
            break;

        case State::DEBOUNCE_CLEAR:
            if (active)
            {
                setState(State::OBJECT_PRESENT);
                break;
            }

            if ((now - _stateStartMillis) >= _config.clearDebounceMs)
            {
                registerRelease(now);
                setState(State::WAIT_OBJECT);
            }
            break;
    }
}

bool IRTrigger::readTrigger()
{
    if (!_triggerPending)
    {
        return false;
    }

    _triggerPending = false;
    return true;
}

bool IRTrigger::readRelease()
{
    if (!_releasePending)
    {
        return false;
    }

    _releasePending = false;
    return true;
}

void IRTrigger::resetCounter(uint32_t value)
{
    _totalCount = value;
    _state = State::WAIT_OBJECT;
    _objectPresent = false;
    clearLatchedEvents();
    _stateStartMillis = millis();
}

IRTrigger::State IRTrigger::getState() const
{
    return _state;
}

bool IRTrigger::isObjectPresent() const
{
    return _objectPresent;
}

uint32_t IRTrigger::getTotalCount() const
{
    return _totalCount;
}

IRTrigger::DebugData IRTrigger::getDebugData() const
{
    DebugData data;
    data.state = _state;
    data.lastEvent = _lastEvent;
    data.objectPresent = _objectPresent;
    data.totalCount = _totalCount;
    data.lastTriggerTimestamp = _lastTriggerTimestamp;
    data.lastReleaseTimestamp = _lastReleaseTimestamp;
    return data;
}

const char* IRTrigger::stateToString(State state) const
{
    switch (state)
    {
        case State::WAIT_OBJECT:
            return "WAIT_OBJECT";

        case State::DEBOUNCE_TRIGGER:
            return "DEBOUNCE_TRIGGER";

        case State::OBJECT_PRESENT:
            return "OBJECT_PRESENT";

        case State::DEBOUNCE_CLEAR:
            return "DEBOUNCE_CLEAR";

        default:
            return "UNKNOWN";
    }
}

const char* IRTrigger::eventToString(Event event) const
{
    switch (event)
    {
        case Event::NONE:
            return "NONE";

        case Event::TRIGGERED:
            return "TRIGGERED";

        case Event::RELEASED:
            return "RELEASED";

        default:
            return "UNKNOWN";
    }
}

void IRTrigger::printDebug(Stream& stream) const
{
    const DebugData data = getDebugData();

    stream.println(F("========== IRTrigger =========="));
    stream.print(F("State : "));
    stream.println(stateToString(data.state));
    stream.print(F("Event : "));
    stream.println(eventToString(data.lastEvent));
    stream.print(F("Object Present : "));
    stream.println(data.objectPresent ? F("true") : F("false"));
    stream.print(F("Total Count : "));
    stream.println(data.totalCount);
    stream.print(F("Last Trigger Ms : "));
    stream.println(data.lastTriggerTimestamp);
    stream.print(F("Last Release Ms : "));
    stream.println(data.lastReleaseTimestamp);
    stream.println(F("======================================"));
}

bool IRTrigger::readSensorActive() const
{
    const int level = digitalRead(_config.sensorPin);

    if (_config.activeLow)
    {
        return (level == LOW);
    }

    return (level == HIGH);
}

void IRTrigger::setState(State newState)
{
    _state = newState;
    _stateStartMillis = millis();
}

void IRTrigger::registerTrigger(uint32_t now)
{
    _objectPresent = true;
    _totalCount++;
    _lastTriggerTimestamp = now;
    _lastEvent = Event::TRIGGERED;
    _triggerPending = true;
    _releasePending = false;
}

void IRTrigger::registerRelease(uint32_t now)
{
    _objectPresent = false;
    _lastReleaseTimestamp = now;
    _lastEvent = Event::RELEASED;
    _releasePending = true;
    _triggerPending = false;
}

void IRTrigger::clearLatchedEvents()
{
    _triggerPending = false;
    _releasePending = false;
    _lastEvent = Event::NONE;
}
