/**
 * ============================================================
 * IoTBlynk.cpp
 * ============================================================
 * Blynk Telemetry Library
 * Platform : ESP32
 * ============================================================
 */

#include "IoTBlynk.h"
#include <BlynkSimpleEsp32.h>

IoTBlynk::IoTBlynk(
    const Config& config
)
:
_config(config),
_state(IoTState::DISCONNECTED),
_lastUploadTime(0),
_lastColor("UNKNOWN"),
_systemState("IDLE"),
_objectCount(0),
_emergency(false)
{
}

IoTBlynk::Config IoTBlynk::defaultConfig()
{
    Config config;

    config.authToken = "TOKEN";
    config.ssid = "WIFI_NAME";
    config.password = "WIFI_PASS";
    config.uploadIntervalMs = IoTBlynkConfig::DEFAULT_UPLOAD_INTERVAL_MS;

    return config;
}

bool IoTBlynk::begin()
{
    WiFi.begin(
        _config.ssid,
        _config.password
    );

    Blynk.config(
        _config.authToken
    );

    _state =
        IoTState::CONNECTING;

    return true;
}

void IoTBlynk::update()
{
    if(WiFi.status() ==
       WL_CONNECTED)
    {
        if(!Blynk.connected())
        {
            Blynk.connect();
        }

        Blynk.run();

        _state =
            IoTState::CONNECTED;
    }
    else
    {
        _state =
            IoTState::DISCONNECTED;
    }

    uint32_t now =
        millis();

    if(now - _lastUploadTime >=
       _config.uploadIntervalMs)
    {
        _lastUploadTime = now;

        sendTelemetry();
    }
}

void IoTBlynk::sendTelemetry()
{
    if(!Blynk.connected())
    {
        return;
    }

    Blynk.virtualWrite(
        V0,
        _systemState
    );

    Blynk.virtualWrite(
        V1,
        _lastColor
    );

    Blynk.virtualWrite(
        V2,
        _objectCount
    );

    Blynk.virtualWrite(
        V3,
        _emergency
    );
}

void IoTBlynk::setLastColor(
    const String& color
)
{
    _lastColor = color;
}

void IoTBlynk::setObjectCount(
    uint32_t count
)
{
    _objectCount = count;
}

void IoTBlynk::setEmergency(
    bool state
)
{
    _emergency = state;
}

void IoTBlynk::setSystemState(
    const String& state
)
{
    _systemState = state;
}

IoTState IoTBlynk::getState() const
{
    return _state;
}

IoTBlynk::DebugData
IoTBlynk::getDebugData() const
{
    DebugData data;

    data.state =
        _state;

    data.wifiConnected =
        (WiFi.status() ==
         WL_CONNECTED);

    data.blynkConnected =
        Blynk.connected();

    data.objectCount =
        _objectCount;

    data.emergency =
        _emergency;

    return data;
}

void IoTBlynk::printDebug(
    Stream& stream
) const
{
    stream.println(
        F("===== IoTBlynk =====")
    );

    stream.print(
        F("State : ")
    );

    stream.println(
        stateToString(_state)
    );

    stream.print(
        F("Objects : ")
    );

    stream.println(
        _objectCount
    );

    stream.print(
        F("Emergency : ")
    );

    stream.println(
        _emergency
    );

    stream.println(
        F("====================")
    );
}

const char*
IoTBlynk::stateToString(
    IoTState state
) const
{
    switch(state)
    {
        case IoTState::DISCONNECTED:
            return "DISCONNECTED";

        case IoTState::CONNECTING:
            return "CONNECTING";

        case IoTState::CONNECTED:
            return "CONNECTED";

        default:
            return "UNKNOWN";
    }
}