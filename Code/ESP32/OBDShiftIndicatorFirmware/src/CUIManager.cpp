#include "CUIManager.hpp"
#include "CDataPool.hpp"
#include "DConfig.hpp"
#include <Arduino.h>
#include <EEPROM.h>

CUIManager::CUIManager()
{
    EEPROM.begin(EEPROM_STORAGE_SIZE);
    CDataPool::Instance()->SetData(SETTINGS_SHIFTLIGHTS_ENABLED, EEPROM.read(EEPROM_SHIFTLIGHT_ACTIVE_BYTE));
    CDataPool::Instance()->SetData(SETTINGS_SHIFTLIGHT_BRIGHTNESS, EEPROM.read(EEPROM_SHIFTLIGHT_BRIGHTNESS_BYTE));
 
    _shiftLedTask = new CShiftLedTask(eShifterType::NORMAL, eShiftMode::MODE0);
    _shiftLedTask->StartTask();
}

CUIManager::~CUIManager()
{
}

void CUIManager::SetShifterActive(bool isActive)
{
    if (isActive)
    {
        CDataPool::Instance()->SetData(SETTINGS_SHIFTLIGHTS_ENABLED, 1);
        EEPROM.write(EEPROM_SHIFTLIGHT_ACTIVE_BYTE, 1);
        EEPROM.commit();
    }
    else
    {
        CDataPool::Instance()->SetData(SETTINGS_SHIFTLIGHTS_ENABLED, 0);
        EEPROM.write(EEPROM_SHIFTLIGHT_ACTIVE_BYTE, 0);
        EEPROM.commit();
    }
}

void CUIManager::SetShifterBrightness(char newShifterBrightness)
{
    if (CDataPool::Instance()->GetData(SETTINGS_SHIFTLIGHT_BRIGHTNESS) != newShifterBrightness)
    {
        //brightness has changed!
        CDataPool::Instance()->SetData(SETTINGS_SHIFTLIGHT_BRIGHTNESS, newShifterBrightness);
        EEPROM.write(EEPROM_SHIFTLIGHT_BRIGHTNESS_BYTE, newShifterBrightness);
        EEPROM.commit();
        _shiftLedTask->NotifyNewShifterBrightnessIsSet();
    }
}