/**
 * ============================================================
 * ColorSensor.cpp
 * ============================================================
 */

#include "ColorSensor.h"

const ColorSensor::ColorReference
ColorSensor::_database[COLOR_COUNT] =
{
    { ColorID::RED,   0.4765657987f, 0.2736101337f, 0.2498240676f },
    { ColorID::RED,   0.4765801354f, 0.2731376975f, 0.2502821670f },
    { ColorID::RED,   0.4763918252f, 0.2734319944f, 0.2501761804f },
    { ColorID::RED,   0.4766949153f, 0.2733050847f, 0.2500000000f },
    { ColorID::RED,   0.4766275950f, 0.2732664878f, 0.2501059172f },
    { ColorID::RED,   0.4774366954f, 0.2733059839f, 0.2492573207f },
    { ColorID::RED,   0.4770318021f, 0.2734982332f, 0.2494699647f },
    { ColorID::RED,   0.4774075120f, 0.2732279017f, 0.2493645863f },
    { ColorID::RED,   0.4776710006f, 0.2734595817f, 0.2488694178f },
    { ColorID::RED,   0.4775781582f, 0.2733059839f, 0.2491158580f },
    { ColorID::GREEN, 0.3856379513f, 0.3693470611f, 0.2450149876f },
    { ColorID::GREEN, 0.3856975381f, 0.3685033216f, 0.2457991403f },
    { ColorID::GREEN, 0.3852266806f, 0.3688118812f, 0.2459614382f },
    { ColorID::GREEN, 0.3859007833f, 0.3689295039f, 0.2451697128f },
    { ColorID::GREEN, 0.3857589984f, 0.3682837767f, 0.2459572248f },
    { ColorID::GREEN, 0.3858596400f, 0.3692929820f, 0.2448473780f },
    { ColorID::GREEN, 0.3858206032f, 0.3693693694f, 0.2448100274f },
    { ColorID::GREEN, 0.3856285863f, 0.3691966615f, 0.2451747522f },
    { ColorID::GREEN, 0.3859397418f, 0.3683318117f, 0.2457284466f },
    { ColorID::GREEN, 0.3853976532f, 0.3688396349f, 0.2457627119f },
    { ColorID::BLUE,  0.3034776903f, 0.3262795276f, 0.3702427822f },
    { ColorID::BLUE,  0.3031198686f, 0.3257799672f, 0.3711001642f },
    { ColorID::BLUE,  0.3027311616f, 0.3262586377f, 0.3710102007f },
    { ColorID::BLUE,  0.3037766831f, 0.3261083744f, 0.3701149425f },
    { ColorID::BLUE,  0.3030900723f, 0.3262656147f, 0.3706443130f },
    { ColorID::BLUE,  0.3026662278f, 0.3260368664f, 0.3712969059f },
    { ColorID::BLUE,  0.3029804051f, 0.3268565783f, 0.3701630166f },
    { ColorID::BLUE,  0.3031450683f, 0.3265272518f, 0.3703276799f },
    { ColorID::BLUE,  0.3020266930f, 0.3265776899f, 0.3713956171f },
    { ColorID::BLUE,  0.3020266930f, 0.3265776899f, 0.3713956171f }
};

ColorSensor::Config ColorSensor::defaultConfig()
{
    Config config;

    config.pinLedR = 4;
    config.pinLedG = 2;
    config.pinLedB = 15;
    config.pinPhotodiode = 33;
    config.sampleDelayMs = 50;

    return config;
}

ColorSensor::ColorSensor(const Config& config)
    : _config(config),
      _state(State::IDLE),
      _detectedColor(ColorID::UNKNOWN),
    _previousMillis(0),
    _stateEnteredMillis(0)
{
}


void ColorSensor::begin()
{
    pinMode(_config.pinLedR, OUTPUT);
    Serial.println("pinLedR: " + String(_config.pinLedR));
    pinMode(_config.pinLedG, OUTPUT);
    Serial.println("pinLedG: " + String(_config.pinLedG));
    pinMode(_config.pinLedB, OUTPUT);
    Serial.println("pinLedB: " + String(_config.pinLedB));

    digitalWrite(_config.pinLedR, HIGH);
    digitalWrite(_config.pinLedG, HIGH);
    digitalWrite(_config.pinLedB, HIGH);

    delay(1000);
    
    setAllLedOff();
    Serial.println("All LED OFF");
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

            _stateEnteredMillis = now;
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
            if ((now - _stateEnteredMillis) < SETTLE_DELAY_MS)
            {
                return;
            }

            _raw.ambient =
                analogRead(_config.pinPhotodiode);

            _stateEnteredMillis = 0;
            _state = State::READ_RED;
            break;

        case State::READ_RED:
            if (_stateEnteredMillis == 0)
            {
                digitalWrite(_config.pinLedR, HIGH);
                _stateEnteredMillis = now;
                return;
            }

            if ((now - _stateEnteredMillis) < SETTLE_DELAY_MS)
            {
                return;
            }

            _raw.red =
                analogRead(_config.pinPhotodiode);

            digitalWrite(_config.pinLedR, LOW);

            _stateEnteredMillis = 0;
            _state = State::READ_GREEN;
            break;

        case State::READ_GREEN:
            if (_stateEnteredMillis == 0)
            {
                digitalWrite(_config.pinLedG, HIGH);
                _stateEnteredMillis = now;
                return;
            }

            if ((now - _stateEnteredMillis) < SETTLE_DELAY_MS)
            {
                return;
            }

            _raw.green =
                analogRead(_config.pinPhotodiode);

            digitalWrite(_config.pinLedG, LOW);

            _stateEnteredMillis = 0;
            _state = State::READ_BLUE;
            break;

        case State::READ_BLUE:
            if (_stateEnteredMillis == 0)
            {
                digitalWrite(_config.pinLedB, HIGH);
                _stateEnteredMillis = now;
                return;
            }

            if ((now - _stateEnteredMillis) < SETTLE_DELAY_MS)
            {
                return;
            }

            _raw.blue =
                analogRead(_config.pinPhotodiode);

            digitalWrite(_config.pinLedB, LOW);

            _stateEnteredMillis = 0;
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
