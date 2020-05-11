#ifndef C_CARINFOSYSTEM_HPP
#define C_CARINFOSYSTEM_HPP

#include "CDataPool.hpp"
#include "CCanTask.hpp"
#include "ICanEventCallback.hpp"
#include "CWifiTask.hpp"
#include "CBluetoothLETask.hpp"

namespace eOBDRequestSMS
{
  enum eOBDRequestSMS
  {
    WAITING_RPM_RESPONSE,
    WAITING_ENGINE_OIL_TEMP_RESPONSE,
    REQUESTING_RPM,
    RETRY_REQUESTING_RPM,
    REQUESTING_ENGINE_OIL_TEMP,
    RETRY_REQUESTING_ENGINE_OIL_TEMP
  };
}

class CCarInfoSystem : public ICanEventCallback
{
public:
  CCarInfoSystem();
  ~CCarInfoSystem();

  void StartSystem();

private:
  void NewCanDataEvent(eNewCanDataEvent::eNewCanDataEvent newCanDataEventType);

  void RunTask();

  void RunSM();

  eOBDRequestSMS::eOBDRequestSMS _currentSMS;

  eNewCanDataEvent::eNewCanDataEvent _waitingForCANEvent;

  CCanTask *_canTask;
};

#endif