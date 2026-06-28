#include <Arduino.h>

#include "ColorSensor.h"
#include "ConveyorMotor.h"
#include "EmergencyStop.h"
#include "IRTrigger.h"
#include "IoTBlynk.h"
#include "ServoSorter.h"
#include "UI.h"

namespace
{
  ColorSensor colorSensor(ColorSensor::defaultConfig());
  ServoSorter servoSorter(ServoSorter::defaultConfig());
  ConveyorMotor conveyorMotor(ConveyorMotor::defaultConfig());
  EmergencyStop emergencyStop(EmergencyStop::defaultConfig());
  UI ui(UI::defaultConfig());
  IoTBlynk iot(IoTBlynk::defaultConfig());
  IRTrigger irTrigger(IRTrigger::defaultConfig());

  uint32_t lastDebugMs = 0;
  String lastColorText = "UNKNOWN";
  uint32_t lastObjectCount = 0;
  bool scanRequested = false;

  String systemStateText = "IDLE";

  ServoSorter::ColorClass mapColor(ColorSensor::ColorID color)
  {
    switch (color)
    {
      case ColorSensor::ColorID::RED:
        return ServoSorter::ColorClass::RED;
      case ColorSensor::ColorID::GREEN:
        return ServoSorter::ColorClass::GREEN;
      case ColorSensor::ColorID::BLUE:
        return ServoSorter::ColorClass::BLUE;
      case ColorSensor::ColorID::WHITE:
        return ServoSorter::ColorClass::WHITE;
      case ColorSensor::ColorID::BLACK:
        return ServoSorter::ColorClass::BLACK;
      default:
        return ServoSorter::ColorClass::UNKNOWN;
    }
  }

  String mapSystemStateText()
  {
    if (emergencyStop.isEmergency())
    {
      return "EMERGENCY";
    }

    if (servoSorter.isBusy())
    {
      return "SORTING";
    }

    if (scanRequested)
    {
      return "READING";
    }

    if (conveyorMotor.isRunning())
    {
      return "RUNNING";
    }

    return "STOPPED";
  }
}

void setup()
{
  Serial.begin(115200);

  colorSensor.begin();

  servoSorter.begin();

  conveyorMotor.begin();

  emergencyStop.begin();

  ui.begin();

  irTrigger.begin();

  iot.begin();

  conveyorMotor.start();
  systemStateText = "RUNNING";
  ui.setSystemState(systemStateText);
  iot.setSystemState(systemStateText);
}

void loop()
{
  emergencyStop.update();

  if (emergencyStop.isEmergency())
  {
    conveyorMotor.emergencyStop();
    servoSorter.emergencyStop();
    scanRequested = false;
    systemStateText = "EMERGENCY";
    ui.setSystemState(systemStateText);
    iot.setEmergency(true);
  }
  else
  {
    conveyorMotor.resetEmergency();
    servoSorter.resetEmergency();
    iot.setEmergency(false);
  }

  irTrigger.update();
  servoSorter.update();
  conveyorMotor.update();
  ui.update();
  iot.update();

  if (irTrigger.readTrigger())
  {
    conveyorMotor.stop();
    scanRequested = true;
  }

  if (scanRequested)
  {
    colorSensor.update();

    if (colorSensor.readColor())
    {
      const ColorSensor::ColorID detectedColor = colorSensor.getColor();
      const ServoSorter::ColorClass targetColor = mapColor(detectedColor);

      lastColorText = colorSensor.colorToString(detectedColor);
      lastObjectCount = irTrigger.getTotalCount();

      servoSorter.triggerSort(targetColor);

      ui.setLastColor(lastColorText);
      ui.setObjectCount(lastObjectCount);
      systemStateText = "SORTING";
      ui.setSystemState(systemStateText);
      ui.setTemperature(emergencyStop.getTemperatureC());

      iot.setLastColor(lastColorText);
      iot.setObjectCount(lastObjectCount);
      iot.setSystemState(systemStateText);
      iot.setTemperature(emergencyStop.getTemperatureC());

      conveyorMotor.start();
      scanRequested = false;
    }
  }

  ui.setTemperature(emergencyStop.getTemperatureC());
  iot.setTemperature(emergencyStop.getTemperatureC());
  systemStateText = mapSystemStateText();
  ui.setSystemState(systemStateText);
  iot.setSystemState(systemStateText);

  if ((millis() - lastDebugMs) >= 1000U)
  {
    lastDebugMs = millis();

    Serial.println(F("===== Integrated System ====="));
    Serial.print(F("System State : "));
    Serial.println(systemStateText);
    Serial.print(F("Last Color   : "));
    Serial.println(lastColorText);
    Serial.print(F("Object Count : "));
    Serial.println(lastObjectCount);
    Serial.println(F("============================="));
  }
}