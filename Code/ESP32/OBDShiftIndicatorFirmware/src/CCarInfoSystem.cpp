#include "CCarInfoSystem.hpp"
#include "DConfig.hpp"
#include <EEPROM.h>
#include "CUIManager.hpp"

void CCarInfoSystem::StartSystem()
{
  _canTask = new CCanTask(this);
  _canTask->StartTask();

  _canTask = new CCanTask(this);
  _canTask->StartTask();

  CUIManager *uiManager = new CUIManager();

  if (IS_OTA_FUNCTIONALITY_ENABLED)
  {
    CWifiTask *wifiConfigTask = new CWifiTask();
    wifiConfigTask->StartTask();
  }

  CBluetoothLETask *bleTask = new CBluetoothLETask(uiManager);
  bleTask->StartTask();

  RunTask();
}

void CCarInfoSystem::NewCanDataEvent(eNewCanDataEvent::eNewCanDataEvent newCanDataEventType)
{
  if (newCanDataEventType == _waitingForCANEvent)
  {
    _waitingForCANEvent = eNewCanDataEvent::NO_EVENT;
  }
}

CCarInfoSystem::CCarInfoSystem()
{
  _waitingForCANEvent = eNewCanDataEvent::NO_EVENT;
  _currentSMS = eOBDRequestSMS::REQUESTING_RPM;
}

void CCarInfoSystem::RunSM()
{
  switch (_currentSMS)
  {
  case eOBDRequestSMS::REQUESTING_RPM:
    _canTask->SendOBDPID(1, MODE_01, PID_RPM);
    _waitingForCANEvent = eNewCanDataEvent::EVENT_RPM;
    _currentSMS = eOBDRequestSMS::WAITING_RPM_RESPONSE;
    break;
  case eOBDRequestSMS::REQUESTING_ENGINE_OIL_TEMP:
    _canTask->SendOBDPID(1, MODE_01, PID_ENGINE_OIL_TEMP);
    _waitingForCANEvent = eNewCanDataEvent::EVENT_ENGINE_OIL_TEMP;
    _currentSMS = eOBDRequestSMS::WAITING_ENGINE_OIL_TEMP_RESPONSE;
    break;
  case eOBDRequestSMS::WAITING_RPM_RESPONSE:
    if (_waitingForCANEvent == eNewCanDataEvent::NO_EVENT)
    {
      _currentSMS = eOBDRequestSMS::REQUESTING_ENGINE_OIL_TEMP;
    }
    else
    {
      _currentSMS = eOBDRequestSMS::RETRY_REQUESTING_RPM;
    }
    break;
  case eOBDRequestSMS::WAITING_ENGINE_OIL_TEMP_RESPONSE:
    if (_waitingForCANEvent == eNewCanDataEvent::NO_EVENT)
    {
      _currentSMS = eOBDRequestSMS::REQUESTING_RPM;
    }
    else
    {
      _currentSMS = eOBDRequestSMS::RETRY_REQUESTING_ENGINE_OIL_TEMP;
    }
    break;
  case eOBDRequestSMS::RETRY_REQUESTING_RPM:
    _canTask->SendOBDPID(1, MODE_01, PID_RPM);
    _currentSMS = eOBDRequestSMS::WAITING_RPM_RESPONSE;
    break;
  case eOBDRequestSMS::RETRY_REQUESTING_ENGINE_OIL_TEMP:
    _canTask->SendOBDPID(1, MODE_01, PID_ENGINE_OIL_TEMP);
    _currentSMS = eOBDRequestSMS::WAITING_ENGINE_OIL_TEMP_RESPONSE;
    break;
  }
}

void CCarInfoSystem::RunTask()
{
  int baseTimeSlice = 20; //ms loop
  int baseTimeSliceCounter = 0;

  int timer20ms = 20 / baseTimeSlice;

  while (1)
  {
    if (baseTimeSliceCounter % timer20ms == 0)
    {
      RunSM();
    }

    baseTimeSliceCounter++;
    delay(baseTimeSlice);
    yield();
  }
}