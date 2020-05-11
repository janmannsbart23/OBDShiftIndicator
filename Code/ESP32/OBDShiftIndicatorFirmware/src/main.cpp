#include <Arduino.h>
#include "CCarInfoSystem.hpp"
#include "DConfig.hpp"
#include <EEPROM.h>

void setup()
{
  Serial.begin(115200);

  if (FIRST_LAUNCH_SETTINGS_OVERRIDE)
  {
    EEPROM.begin(EEPROM_STORAGE_SIZE);
    EEPROM.write(EEPROM_SHIFTLIGHT_ACTIVE_BYTE, 1);
    EEPROM.write(EEPROM_SHIFTLIGHT_BRIGHTNESS_BYTE, 128);
    EEPROM.commit();

    while (true)
    {
      Serial.println("EEPROM values overwritten! Change 'FIRST_LAUNCH_SETTINGS_OVERRIDE' to false again!");
      delay(1000);
    }
  }

  CCarInfoSystem *carInfoSystem = new CCarInfoSystem();
  carInfoSystem->StartSystem();
}

void loop()
{
  delay(1000);
}
