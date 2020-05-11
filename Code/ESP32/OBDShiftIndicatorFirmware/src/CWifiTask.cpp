#include "CWifiTask.hpp"
#include "DConfig.hpp"
#include "CDataPool.hpp"
#include "EPINDefines.h"
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include "CWifiCredentials.hpp"

CWifiTask::CWifiTask()
{
}

CWifiTask::~CWifiTask()
{
}

void CWifiTask::StartTask()
{
  xTaskCreatePinnedToCore(this->StartTaskImpl, "Wifi Website Task", 4096, this, WIFI_CONFIG_WEBSITE_TASK_PRIO, NULL, WIFI_CONFIG_WEBSITE_CORE);
}

void CWifiTask::ReconnectWifi()
{
  int baseTimeSlice = 10; //ms loop
  int baseTimeSliceCounter = 0;

  int timer3000ms = 3000 / baseTimeSlice;

  while (WiFi.status() != WL_CONNECTED)
  {

    if (baseTimeSliceCounter % timer3000ms == 0)
    {
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      // delay(200);
      WiFi.mode(WIFI_STA);
      delay(200);
      WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
      WiFi.begin(SSID, PASSWORD);
      WiFi.setHostname("ShiftLightIndicator");
      yield();
    }
    baseTimeSliceCounter++;
    delay(baseTimeSlice);
    yield();
  }
}

void CWifiTask::Task()
{

  ReconnectWifi();

  ArduinoOTA.begin();

  while (1)
  {

    if (WiFi.status() != WL_CONNECTED)
    {
      ReconnectWifi();
    }

    else
    {
      ArduinoOTA.handle();
    }
    delay(50);
  }
}

void CWifiTask::StartTaskImpl(void *_this)
{
  static_cast<CWifiTask *>(_this)->Task();
}
