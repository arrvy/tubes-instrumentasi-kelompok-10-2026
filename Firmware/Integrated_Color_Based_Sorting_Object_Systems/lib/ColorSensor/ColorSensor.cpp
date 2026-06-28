/**
 * ============================================================
 * ColorSensor.cpp
 * ============================================================
 */

#include "ColorSensor.h"

const ColorSensor::ColorReference
ColorSensor::_database[COLOR_COUNT] =
{
    { ColorID::RED,   0.30f, 0.60f, 0.72f },
    { ColorID::GREEN, 0.65f, 0.25f, 0.60f },
    { ColorID::BLUE,  0.70f, 0.65f, 0.25f },
    { ColorID::WHITE, 0.33f, 0.33f, 0.33f },
    { ColorID::BLACK, 0.00f, 0.00f, 0.00f }
};

ColorSensor::Config ColorSensor::defaultConfig()
{
    Config config;

    config.pinLedR = 15;
    config.pinLedG = 2;
    config.pinLedB = 4;
    config.pinPhotodiode = 33;
    config.sampleDelayMs = 250;

    return config;
}

ColorSensor::ColorSensor(const Config& config)
    : _config(config),
      _state(State::IDLE),
      _detectedColor(ColorID::UNKNOWN),
      _previousMillis(0)
{
}


void ColorSensor::begin()
{
    pinMode(_config.pinLedR, OUTPUT);
    pinMode(_config.pinLedG, OUTPUT);
    pinMode(_config.pinLedB, OUTPUT);

    setAllLedOff();
}

void ColorSensor::update()
{
    uint32_t now = millis();

    if ((now - _previousMillis) < _config.sampleDelayMs)
    {
        return;
    }

    _previousMillis = now;

    switch (_state)
    {
        case State::IDLE:

            setAllLedOff();

            _state = State::READ_AMBIENT;
            break;

        case State::READ_AMBIENT:
            /**
             * Ambient light measurement.
             *
             * Semua LED dimatikan untuk mengukur cahaya lingkungan
             * yang masuk ke photodiode. Nilai ini digunakan sebagai
             * kompensasi agar hasil pembacaan tidak terlalu terpengaruh
             * oleh perubahan kondisi pencahayaan ruangan.
             */
            _raw.ambient =
                analogRead(_config.pinPhotodiode);

            _state = State::PREP_RED;
            break;

        case State::PREP_RED:

            digitalWrite(_config.pinLedR, HIGH);

            _state = State::READ_RED;
            break;

        case State::READ_RED:

            _raw.red =
                analogRead(_config.pinPhotodiode);

            digitalWrite(_config.pinLedR, LOW);

            _state = State::PREP_GREEN;
            break;

        case State::PREP_GREEN:

            digitalWrite(_config.pinLedG, HIGH);

            _state = State::READ_GREEN;
            break;

        case State::READ_GREEN:

            _raw.green =
                analogRead(_config.pinPhotodiode);

            digitalWrite(_config.pinLedG, LOW);

            _state = State::PREP_BLUE;
            break;

        case State::PREP_BLUE:

            digitalWrite(_config.pinLedB, HIGH);

            _state = State::READ_BLUE;
            break;

        case State::READ_BLUE:

            _raw.blue =
                analogRead(_config.pinPhotodiode);

            digitalWrite(_config.pinLedB, LOW);

            _state = State::PROCESS;
            break;

        case State::PROCESS:

            processClassification();

            _state = State::DONE;
            break;

        case State::DONE:
            break;
    }
}

bool ColorSensor::readColor()
{
    if (_state != State::DONE)
    {
        return false;
    }

    _state = State::IDLE;

    return true;
}

void ColorSensor::processClassification()
{
    float r =
        max(0, (int)_raw.red - (int)_raw.ambient);

    float g =
        max(0, (int)_raw.green - (int)_raw.ambient);

    float b =
        max(0, (int)_raw.blue - (int)_raw.ambient);

    float total = r + g + b;

    if (total <= 0.01f)
    {
        _detectedColor = ColorID::UNKNOWN;
        return;
    }

    _normalized.r = r / total;
    _normalized.g = g / total;
    _normalized.b = b / total;

    float bestDistance = 9999.0f;

    ColorID bestColor = ColorID::UNKNOWN;

    for (uint8_t i = 0; i < COLOR_COUNT; i++)
    {
        float distance =
            calculateDistance(
                _normalized,
                _database[i]);

        if (distance < bestDistance)
        {
            bestDistance = distance;
            bestColor = _database[i].id;
        }
    }

    _detectedColor = bestColor;
}

float ColorSensor::calculateDistance(
    const RGBNormalized& sample,
    const ColorReference& ref)
{
    float dr = sample.r - ref.r;
    float dg = sample.g - ref.g;
    float db = sample.b - ref.b;

    return sqrt(
        dr * dr +
        dg * dg +
        db * db);
}

void ColorSensor::setAllLedOff()
{
    digitalWrite(_config.pinLedR, LOW);
    digitalWrite(_config.pinLedG, LOW);
    digitalWrite(_config.pinLedB, LOW);
}

ColorSensor::ColorID
ColorSensor::getColor() const
{
    return _detectedColor;
}

ColorSensor::RGBRaw
ColorSensor::getRawData() const
{
    return _raw;
}

ColorSensor::RGBNormalized
ColorSensor::getNormalizedData() const
{
    return _normalized;
}

ColorSensor::State
ColorSensor::getState() const
{
    return _state;
}

const char* ColorSensor::colorToString(
    ColorID color) const
{
    switch (color)
    {
        case ColorID::RED:
            return "RED";

        case ColorID::GREEN:
            return "GREEN";

        case ColorID::BLUE:
            return "BLUE";

        case ColorID::WHITE:
            return "WHITE";

        case ColorID::BLACK:
            return "BLACK";

        default:
            return "UNKNOWN";
    }
}
