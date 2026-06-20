/**
 * ============================================================
 * ConveyorMotor.cpp
 * ============================================================
 * Conveyor Motor Library
 * Platform : ESP32
 * ============================================================
 */

#include "ConveyorMotor.h"

ConveyorMotor::ConveyorMotor(
    const Config& config
)
:
_config(config),
_state(ConveyorState::STOPPED),
_speed(config.defaultSpeed)
{
}

bool ConveyorMotor::begin()
{
    pinMode(_config.in1Pin, OUTPUT);
    pinMode(_config.in2Pin, OUTPUT);

    // Configure PWM channel 0 with the specified frequency and resolution
    ledcSetup(0, _config.pwmFrequency, _config.pwmResolution);
    // Attach the enable pin to channel 0
    ledcAttachPin(_config.enPin, 0);

    stop();

    return true;
}

void ConveyorMotor::update()
{
    applyOutput();
}

void ConveyorMotor::start()
{
    if(_state == ConveyorState::EMERGENCY)
    {
        return;
    }

    _state = ConveyorState::RUNNING;
}

void ConveyorMotor::stop()
{
    _state = ConveyorState::STOPPED;
}

void ConveyorMotor::emergencyStop()
{
    _state = ConveyorState::EMERGENCY;
}

void ConveyorMotor::resetEmergency()
{
    _state = ConveyorState::STOPPED;
}

void ConveyorMotor::setSpeed(
    uint8_t speed
)
{
    _speed = speed;
}

uint8_t ConveyorMotor::getSpeed() const
{
    return _speed;
}

ConveyorState ConveyorMotor::getState() const
{
    return _state;
}

bool ConveyorMotor::isRunning() const
{
    return (_state == ConveyorState::RUNNING);
}

ConveyorMotor::DebugData
ConveyorMotor::getDebugData() const
{
    DebugData data;

    data.state = _state;

    data.speed = _speed;

    return data;
}

void ConveyorMotor::printDebug(
    Stream& stream
) const
{
    stream.println(F("===== ConveyorMotor ====="));

    stream.print(F("State : "));
    stream.println(
        stateToString(_state)
    );

    stream.print(F("Speed : "));
    stream.println(_speed);

    stream.println(F("========================="));
}

void ConveyorMotor::applyOutput()
{
    switch(_state)
    {
        case ConveyorState::RUNNING:
        {
            digitalWrite(
                _config.in1Pin,
                HIGH
            );

            digitalWrite(
                _config.in2Pin,
                LOW
            );

            ledcWrite(
                0,
                _speed
            );

            break;
        }

        case ConveyorState::STOPPED:
        {
            ledcWrite(
                0,
                0
            );

            break;
        }

        case ConveyorState::EMERGENCY:
        {
            digitalWrite(
                _config.in1Pin,
                LOW
            );

            digitalWrite(
                _config.in2Pin,
                LOW
            );

            ledcWrite(
                0,
                0
            );

            break;
        }

        default:
        {
            break;
        }
    }
}

const char* ConveyorMotor::stateToString(
    ConveyorState state
) const
{
    switch(state)
    {
        case ConveyorState::STOPPED:
            return "STOPPED";

        case ConveyorState::RUNNING:
            return "RUNNING";

        case ConveyorState::EMERGENCY:
            return "EMERGENCY";

        default:
            return "UNKNOWN";
    }
}