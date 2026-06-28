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
  bool emergencyActive = false;

  String systemStateText = "RUNNING";

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
  ui.setLastColor(lastColorText);
  ui.setObjectCount(lastObjectCount);
  ui.setTemperature(emergencyStop.getTemperatureC());
  ui.update();
  iot.setSystemState(systemStateText);
  iot.setLastColor(lastColorText);
  iot.setObjectCount(lastObjectCount);
  iot.setTemperature(emergencyStop.getTemperatureC());
}

void loop()
{
  emergencyStop.update();

  const bool emergencyNow = emergencyStop.isEmergency();

  if (emergencyNow)
  {
    if (!emergencyActive)
    {
      conveyorMotor.emergencyStop();
      servoSorter.emergencyStop();
      scanRequested = false;
      iot.setEmergency(true);
      emergencyActive = true;
    }

    systemStateText = "EMERGENCY";
    ui.setSystemState(systemStateText);
  }
  else
  {
    if (emergencyActive)
    {
      emergencyStop.resetEmergency();
      conveyorMotor.resetEmergency();
      servoSorter.resetEmergency();
      conveyorMotor.start();
      iot.setEmergency(false);
      emergencyActive = false;
    }
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
      const bool sortAccepted = servoSorter.triggerSort(targetColor);

      lastColorText = colorSensor.colorToString(detectedColor);
      lastObjectCount = irTrigger.getTotalCount();

      ui.setLastColor(lastColorText);
      ui.setObjectCount(lastObjectCount);
      systemStateText = sortAccepted ? "SORTING" : "SORT_REJECTED";
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