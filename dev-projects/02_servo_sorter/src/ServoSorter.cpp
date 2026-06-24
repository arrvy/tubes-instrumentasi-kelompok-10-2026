    /**
     * ============================================================
     * ServoSorter.cpp
     * ============================================================
     * Single Servo Color Sorting Library
     * Platform : ESP32
     * ============================================================
     */

    #include "ServoSorter.h"
    #include "ServoSorterConfig.h"

    ServoSorter::ServoSorter(const Config& config)
    :
    _config(config),
    _state(State::IDLE),
    _activeColor(ColorClass::UNKNOWN),
    _targetAngle(config.homeAngle),
    _stateTimestamp(0),
    _head(0),
    _tail(0),
    _count(0)
    {
    }

ServoSorter::Config ServoSorter::defaultConfig()
{
    Config config;

    config.servoPin = 18;
    config.homeAngle = ServoSorterConfig::DEFAULT_HOME_ANGLE;
    config.redAngle = ServoSorterConfig::DEFAULT_RED_ANGLE;
    config.greenAngle = ServoSorterConfig::DEFAULT_GREEN_ANGLE;
    config.blueAngle = ServoSorterConfig::DEFAULT_BLUE_ANGLE;
    config.whiteAngle = ServoSorterConfig::DEFAULT_WHITE_ANGLE;
    config.blackAngle = ServoSorterConfig::DEFAULT_BLACK_ANGLE;
    config.sortDelayMs = ServoSorterConfig::DEFAULT_SORT_DELAY_MS;
    config.holdTimeMs = ServoSorterConfig::DEFAULT_HOLD_TIME_MS;

    return config;
}

    bool ServoSorter::begin()
    {
        _servo.attach(_config.servoPin);

        _servo.write(_config.homeAngle);

        return true;
    }

    void ServoSorter::update()
    {
        
        processStateMachine();
    }

    bool ServoSorter::triggerSort(ColorClass color)
    {
        if (_state == State::EMERGENCY)
        {
            return false;
        }

        return enqueue(color);
    }

    void ServoSorter::emergencyStop()
    {
        _servo.write(_config.homeAngle);

        _activeColor = ColorClass::UNKNOWN;

        _count = 0;
        _head = 0;
        _tail = 0;

        _state = State::EMERGENCY;
    }

    void ServoSorter::resetEmergency()
    {
        _servo.write(_config.homeAngle);

        _state = State::IDLE;
    }

    bool ServoSorter::isBusy() const
    {
        return (_state != State::IDLE);
    }

    ServoSorter::State ServoSorter::getState() const
    {
        return _state;
    }

    ServoSorter::ColorClass ServoSorter::getCurrentColor() const
    {
        return _activeColor;
    }

    ServoSorter::DebugData ServoSorter::getDebugData() const
    {
        DebugData data;

        data.state       = _state;
        data.activeColor = _activeColor;
        data.queueCount  = _count;
        data.targetAngle = _targetAngle;

        return data;
    }

    void ServoSorter::printDebug(Stream& stream) const
    {
        stream.println(F("========== ServoSorter =========="));

        stream.print(F("State : "));
        stream.println(stateToString(_state));

        stream.print(F("Color : "));
        stream.println(colorToString(_activeColor));

        stream.print(F("Queue : "));
        stream.println(_count);

        stream.print(F("Target Angle : "));
        stream.println(_targetAngle);

        stream.println(F("================================="));
    }

    bool ServoSorter::enqueue(ColorClass color)
    {
        if (_count >= ServoSorterConfig::MAX_QUEUE_SIZE)
        {
            return false;
        }

        _queue[_tail].color = color;

        _tail++;

        if (_tail >= ServoSorterConfig::MAX_QUEUE_SIZE)
        {
            _tail = 0;
        }

        _count++;

        return true;
    }

    bool ServoSorter::dequeue(SortJob& job)
    {
        if (_count == 0)
        {
            return false;
        }

        job = _queue[_head];

        _head++;

        if (_head >= ServoSorterConfig::MAX_QUEUE_SIZE)
        {
            _head = 0;
        }

        _count--;

        return true;
    }

    int ServoSorter::getTargetAngle(ColorClass color) const
    {
        switch(color)
        {
            case ColorClass::RED:
                return _config.redAngle;

            case ColorClass::GREEN:
                return _config.greenAngle;

            case ColorClass::BLUE:
                return _config.blueAngle;

            case ColorClass::WHITE:
                return _config.whiteAngle;

            case ColorClass::BLACK:
                return _config.blackAngle;

            default:
                return _config.homeAngle;
        }
    }

    void ServoSorter::processStateMachine()
    {
        uint32_t now = millis();

        switch(_state)
        {
            case State::IDLE:
            {
                SortJob job;

                if (dequeue(job))
                {
                    _activeColor = job.color;

                    _targetAngle = getTargetAngle(job.color);

                    _stateTimestamp = now;

                    _state = State::WAIT_DELAY;
                }

                break;
            }

            /*
            * Wait for object travel time from
            * color sensing zone to sorting zone.
            */
            case State::WAIT_DELAY:
            {
                if ((now - _stateTimestamp) >= _config.sortDelayMs)
                {
                    _state = State::MOVE_TO_TARGET;
                }

                break;
            }

            /*
            * Move servo to target angle
            * corresponding to detected color.
            */
            case State::MOVE_TO_TARGET:
            {
                _servo.write(_targetAngle);

                _stateTimestamp = now;

                _state = State::HOLD_POSITION;

                break;
            }

            /*
            * Hold sorting gate for a short duration
            * to ensure object leaves conveyor.
            */
            case State::HOLD_POSITION:
            {
                if ((now - _stateTimestamp) >= _config.holdTimeMs)
                {
                    _state = State::RETURN_HOME;
                }

                break;
            }

            /*
            * Return servo to neutral position
            * and prepare next sorting job.
            */
            case State::RETURN_HOME:
            {
                _servo.write(_config.homeAngle);

                _activeColor = ColorClass::UNKNOWN;

                _state = State::IDLE;

                break;
            }

            case State::EMERGENCY:
            {
                break;
            }

            default:
            {
                break;
            }
        }
    }

    const char* ServoSorter::stateToString(State state) const
    {
        switch(state)
        {
            case State::IDLE:
                return "IDLE";

            case State::WAIT_DELAY:
                return "WAIT_DELAY";

            case State::MOVE_TO_TARGET:
                return "MOVE_TO_TARGET";

            case State::HOLD_POSITION:
                return "HOLD_POSITION";

            case State::RETURN_HOME:
                return "RETURN_HOME";

            case State::EMERGENCY:
                return "EMERGENCY";

            default:
                return "UNKNOWN";
        }
    }

    const char* ServoSorter::colorToString(ColorClass color) const
    {
        switch(color)
        {
            case ColorClass::RED:
                return "RED";

            case ColorClass::GREEN:
                return "GREEN";

            case ColorClass::BLUE:
                return "BLUE";

            case ColorClass::WHITE:
                return "WHITE";

            case ColorClass::BLACK:
                return "BLACK";

            default:
                return "UNKNOWN";
        }
    }