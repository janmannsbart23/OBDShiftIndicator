#ifndef C_DATAPOOL_HPP
#define C_DATAPOOL_HPP

#define MAX_DATA_CHARS 1
#define MAX_DATA_INTS 20
#define MAX_DATA_FLOATS 1
#define MAX_DATA_DOUBLES 1
#define MAX_DATA_LONG 1

enum eCharData
{
};

enum eIntData
{
  OBD_RPM,
  OBD_SPEED,
  OBD_ENGINE_LOAD,
  OBD_THROTTLE_POSITION,
  OBD_INTAKE_AIR_TEMP,
  OBD_ENGINE_OIL_TEMP,
  OBD_COOLANT_TEMP,
  OBD_ENGINE_COOLANT_TEMP,
  OBD_GEAR,
  OBD_FUEL_LEVEL,
  ESP_BOOT_CNT,
  SETTINGS_SHIFTLIGHTS_ENABLED,
  SETTINGS_SHIFTLIGHT_BRIGHTNESS,
  SETTINGS_INFODISPLAY_MODE,
  STATUS_LED_MODE
};

enum eFloatData
{
};

enum eDoubleData
{
};

enum eLongData
{
};

class CDataPool
{
public:
  static CDataPool *Instance();

  void SetData(eCharData data, char value);
  char GetData(eCharData data);

  void SetData(eIntData data, int value);
  int GetData(eIntData data);

  void SetData(eFloatData data, float value);
  float GetData(eFloatData data);

  void SetData(eDoubleData data, double value);
  double GetData(eDoubleData data);

  void SetData(eLongData data, unsigned long long value);
  unsigned long GetData(eLongData data);

private:
  CDataPool();

  static CDataPool *_instance;

  double _doubleData[MAX_DATA_DOUBLES];
  float _floatData[MAX_DATA_FLOATS];
  unsigned int _intData[MAX_DATA_INTS];
  char _charData[MAX_DATA_CHARS];
  unsigned long long _longData[MAX_DATA_LONG];
};

#endif
