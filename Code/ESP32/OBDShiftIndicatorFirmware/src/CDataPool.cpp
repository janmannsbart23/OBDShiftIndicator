#include "CDataPool.hpp"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static SemaphoreHandle_t floatGuard = NULL;
static SemaphoreHandle_t charGuard = NULL;
static SemaphoreHandle_t intGuard = NULL;
static SemaphoreHandle_t doubleGuard = NULL;
static SemaphoreHandle_t longGuard = NULL;

CDataPool *CDataPool::_instance = 0;

CDataPool::CDataPool()
{

  floatGuard = xSemaphoreCreateMutex();
  charGuard = xSemaphoreCreateMutex();
  intGuard = xSemaphoreCreateMutex();
  doubleGuard = xSemaphoreCreateMutex();
  longGuard = xSemaphoreCreateMutex();

  memset(_floatData, 0x0, sizeof(float) * MAX_DATA_FLOATS);
  memset(_charData, 0x0, sizeof(char) * MAX_DATA_CHARS);
  memset(_intData, 0x0, sizeof(int) * MAX_DATA_INTS);
  memset(_doubleData, 0x0, sizeof(unsigned int) * MAX_DATA_DOUBLES);
  memset(_longData, 0x0, sizeof(unsigned long long) * MAX_DATA_LONG);
}

CDataPool *CDataPool::Instance()
{
  if (!_instance)
  {
    _instance = new CDataPool();
  }
  return _instance;
}

void CDataPool::SetData(eCharData data, char value)
{
  xSemaphoreTake(charGuard, portMAX_DELAY);
  _charData[data] = value;
  xSemaphoreGive(charGuard);
}

char CDataPool::GetData(eCharData data)
{
  xSemaphoreTake(charGuard, portMAX_DELAY);
  char tmp = _charData[data];
  xSemaphoreGive(charGuard);
  return tmp;
}

void CDataPool::SetData(eIntData data, int value)
{
  xSemaphoreTake(intGuard, portMAX_DELAY);
  _intData[data] = value;
  xSemaphoreGive(intGuard);
}

int CDataPool::GetData(eIntData data)
{
  xSemaphoreTake(intGuard, portMAX_DELAY);
  int tmp = _intData[data];
  xSemaphoreGive(intGuard);
  return tmp;
}

void CDataPool::SetData(eFloatData data, float value)
{
  xSemaphoreTake(floatGuard, portMAX_DELAY);
  _floatData[data] = value;
  xSemaphoreGive(floatGuard);
}

float CDataPool::GetData(eFloatData data)
{
  xSemaphoreTake(floatGuard, portMAX_DELAY);
  float tmp = _floatData[data];
  xSemaphoreGive(floatGuard);
  return tmp;
}

void CDataPool::SetData(eDoubleData data, double value)
{
  xSemaphoreTake(doubleGuard, portMAX_DELAY);
  _doubleData[data] = value;
  xSemaphoreGive(doubleGuard);
}

double CDataPool::GetData(eDoubleData data)
{
  xSemaphoreTake(doubleGuard, portMAX_DELAY);
  double tmp = _doubleData[data];
  xSemaphoreGive(doubleGuard);
  return tmp;
}

void CDataPool::SetData(eLongData data, unsigned long long value)
{
  xSemaphoreTake(longGuard, portMAX_DELAY);
  _longData[data] = value;
  xSemaphoreGive(longGuard);
}

unsigned long CDataPool::GetData(eLongData data)
{
  xSemaphoreTake(longGuard, portMAX_DELAY);
  long tmp = _longData[data];
  xSemaphoreGive(longGuard);
  return tmp;
}
