#include <Arduino.h>
#include "ConveyorMotor.h"

ConveyorMotor::Config conveyorConfig
{
    .enPin = 25,
    .in1Pin = 26,
    .in2Pin = 27,

    .pwmFrequency =
    ConveyorMotorConfig::DEFAULT_PWM_FREQUENCY,

    .pwmResolution =
    ConveyorMotorConfig::DEFAULT_PWM_RESOLUTION,

    .defaultSpeed =
    ConveyorMotorConfig::DEFAULT_SPEED
};

ConveyorMotor conveyor(
    conveyorConfig
);

void setup()
{
    Serial.begin(115200);

    conveyor.begin();

    conveyor.start();
}

void loop()
{
    conveyor.update();

    if(Serial.available())
    {
        String cmd =
        Serial.readStringUntil('\n');

        cmd.trim();

        if(cmd == "start")
        {
            conveyor.start();
        }

        else if(cmd == "stop")
        {
            conveyor.stop();
        }

        else if(cmd == "debug")
        {
            conveyor.printDebug(
                Serial
            );
        }

        else if(cmd == "emg")
        {
            conveyor.emergencyStop();
        }
    }
}