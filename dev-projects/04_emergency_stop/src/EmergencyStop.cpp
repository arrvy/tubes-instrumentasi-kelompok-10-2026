/**
 * ============================================================
 * EmergencyStop.cpp
 * ============================================================
 * Emergency Stop Library
 * Platform : ESP32
 * ============================================================
 */

#include "EmergencyStop.h"

EmergencyStop::EmergencyStop(
    const Config& config
)
:
_config(config),
_state(EmergencyState::SAFE),
_temperatureC(NAN),
_temperatureValid(false),
_estopPressed(false),
_feedbackLost(false),
_lastTemperatureReadTime(0),
_dht(config.dhtPin, DHT11)
{
}

EmergencyStop::Config EmergencyStop::defaultConfig()
{
    Config config;

    config.dhtPin = 35;
    config.estopPin = 25;
    config.feedbackPin = 26;
    config.temperatureThresholdC = EmergencyStopConfig::DEFAULT_TEMPERATURE_THRESHOLD_C;
    config.readIntervalMs = EmergencyStopConfig::DEFAULT_READ_INTERVAL_MS;
    config.estopActiveLow = true;
    config.feedbackActiveLow = true;

    return config;
}

bool EmergencyStop::begin()
{
    _dht.begin();

    pinMode(
        _config.estopPin,
        INPUT_PULLUP
    );

    pinMode(
        _config.feedbackPin,
        INPUT_PULLUP
    );

    return true;
}

void EmergencyStop::update()
{
    processSafety();
}

void EmergencyStop::processSafety()
{
    uint32_t now = millis();

    if(now - _lastTemperatureReadTime >=
       _config.readIntervalMs)
    {
        _lastTemperatureReadTime = now;

        _temperatureC =
            _dht.readTemperature();

        _temperatureValid =
            !isnan(_temperatureC);
    }

    bool estopRaw =
        digitalRead(
            _config.estopPin
        );

    bool feedbackRaw =
        digitalRead(
            _config.feedbackPin
        );

    _estopPressed =
        _config.estopActiveLow ?
        !estopRaw :
        estopRaw;

    _feedbackLost =
        _config.feedbackActiveLow ?
        !feedbackRaw :
        feedbackRaw;

    if(!_temperatureValid)
    {
        _state =
        EmergencyState::EMERGENCY_SENSOR;

        return;
    }

    if(_temperatureC >=
       _config.temperatureThresholdC)
    {
        _state =
        EmergencyState::EMERGENCY_TEMP;

        return;
    }

    if(_estopPressed)
    {
        _state =
        EmergencyState::EMERGENCY_BUTTON;

        return;
    }

    if(_feedbackLost)
    {
        _state =
        EmergencyState::EMERGENCY_FEEDBACK;

        return;
    }

    if(_state == EmergencyState::SAFE)
    {
        return;
    }
}

void EmergencyStop::resetEmergency()
{
    if(!_temperatureValid)
    {
        return;
    }

    if(_temperatureC >=
       _config.temperatureThresholdC)
    {
        return;
    }

    if(_estopPressed)
    {
        return;
    }

    if(_feedbackLost)
    {
        return;
    }

    _state =
        EmergencyState::SAFE;
}

bool EmergencyStop::isEmergency() const
{
    return (_state !=
            EmergencyState::SAFE);
}

EmergencyState
EmergencyStop::getState() const
{
    return _state;
}

float
EmergencyStop::getTemperatureC() const
{
    return _temperatureC;
}

EmergencyStop::DebugData
EmergencyStop::getDebugData() const
{
    DebugData data;

    data.state = _state;

    data.temperatureC = _temperatureC;

    data.estopPressed =
        _estopPressed;

    data.feedbackLost =
        _feedbackLost;

    data.temperatureValid =
        _temperatureValid;

    return data;
}

void EmergencyStop::printDebug(
    Stream& stream
) const
{
    stream.println(
        F("===== EmergencyStop =====")
    );

    stream.print(F("State : "));
    stream.println(
        stateToString(_state)
    );

    stream.print(F("Temp C : "));
    stream.println(_temperatureC);

    stream.print(F("Temp Valid : "));
    stream.println(_temperatureValid);

    stream.print(F("EStop : "));
    stream.println(_estopPressed);

    stream.print(F("Feedback : "));
    stream.println(_feedbackLost);

    stream.println(
        F("=========================")
    );
}

const char*
EmergencyStop::stateToString(
    EmergencyState state
) const
{
    switch(state)
    {
        case EmergencyState::SAFE:
            return "SAFE";

        case EmergencyState::EMERGENCY_TEMP:
            return "EMERGENCY_TEMP";

        case EmergencyState::EMERGENCY_BUTTON:
            return "EMERGENCY_BUTTON";

        case EmergencyState::EMERGENCY_FEEDBACK:
            return "EMERGENCY_FEEDBACK";

        case EmergencyState::EMERGENCY_SENSOR:
            return "EMERGENCY_SENSOR";

        default:
            return "UNKNOWN";
    }
}