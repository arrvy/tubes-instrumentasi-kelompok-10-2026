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
_mq2Value(0),
_estopPressed(false),
_feedbackLost(false),
_lastDebounceTime(0)
{
}

bool EmergencyStop::begin()
{
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
    _mq2Value =
        analogRead(_config.mq2Pin);

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

    if(_mq2Value >=
       _config.mq2Threshold)
    {
        _state =
        EmergencyState::EMERGENCY_MQ2;

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
    if(_mq2Value >=
       _config.mq2Threshold)
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

uint16_t
EmergencyStop::getMQ2Value() const
{
    return _mq2Value;
}

EmergencyStop::DebugData
EmergencyStop::getDebugData() const
{
    DebugData data;

    data.state = _state;

    data.mq2Value = _mq2Value;

    data.estopPressed =
        _estopPressed;

    data.feedbackLost =
        _feedbackLost;

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

    stream.print(F("MQ2 : "));
    stream.println(_mq2Value);

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

        case EmergencyState::EMERGENCY_MQ2:
            return "EMERGENCY_MQ2";

        case EmergencyState::EMERGENCY_BUTTON:
            return "EMERGENCY_BUTTON";

        case EmergencyState::EMERGENCY_FEEDBACK:
            return "EMERGENCY_FEEDBACK";

        default:
            return "UNKNOWN";
    }
}