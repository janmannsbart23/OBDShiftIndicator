#include "CBluetoothLETask.hpp"
#include "DConfig.hpp"
#include "CDataPool.hpp"

CBluetoothLETask::CBluetoothLETask(CUIManager *uiManager)
{
  _uiManager = uiManager;
}

CBluetoothLETask::~CBluetoothLETask()
{
}

void CBluetoothLETask::StartTask()
{
  xTaskCreatePinnedToCore(this->StartTaskImpl, "Bluetooth Task", 4096, this, BLUETOOTH_TASK_PRIO, NULL, BLUETOOTH_CORE);
}

void CBluetoothLETask::Task()
{

  _deviceConnected = false;
  _deviceSuccessfullyReceivedConfig = false;

  BLEDevice::init("SHIFT INDICATOR");
  _pServer = BLEDevice::createServer();

  // Create the BLE Service
  _pService = _pServer->createService(SHIFTINDICATOR_SERVICE);
  _pService2 = _pServer->createService(SHIFTINDICATOR_SERVICE2);

  _rpmDataCharacteristic = _pService->createCharacteristic(
      RPM_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_NOTIFY);

  _engineOilCharacteristic = _pService->createCharacteristic(
      EOIL_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_NOTIFY);

  _configSenderCharacteristic = _pService->createCharacteristic(
      CONFIG_SENDER_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_NOTIFY);

  _shifterActiveReaderCharacteristic = _pService2->createCharacteristic(
      SHIFTER_ACTIVE_READER_UUID,
      BLECharacteristic::PROPERTY_WRITE);

  _shifterBrightnessReaderCharacteristic = _pService2->createCharacteristic(
      SHIFTER_BRIGHTNESS_READER_UUID,
      BLECharacteristic::PROPERTY_WRITE);

  _configReadCharacteristic = _pService->createCharacteristic(
      CONFIG_READER_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE);

  _pServer->setCallbacks(this);

  _rpmDataCharacteristic->addDescriptor(new BLE2902());
  _engineOilCharacteristic->addDescriptor(new BLE2902());
  _configSenderCharacteristic->addDescriptor(new BLE2902());

  _shifterActiveReaderCharacteristic->addDescriptor(new BLE2902());
  _shifterBrightnessReaderCharacteristic->addDescriptor(new BLE2902());
  _shifterActiveReaderCharacteristic->setCallbacks(this);
  _shifterBrightnessReaderCharacteristic->setCallbacks(this);

  _configReadCharacteristic->addDescriptor(new BLE2902());
  _configReadCharacteristic->setCallbacks(this);

  // Start the service
  _pService->start();
  _pService2->start();

  // Start advertising
  _pServer->getAdvertising()->start();

  uint32_t rpmData = 0;
  uint32_t eoilData = 0;

  while (1)
  {
    if (_deviceConnected)
    {
      if (_deviceSuccessfullyReceivedConfig)
      {
        //send data
        rpmData = CDataPool::Instance()->GetData(OBD_RPM);
        eoilData = CDataPool::Instance()->GetData(OBD_ENGINE_OIL_TEMP);
        eoilData += 40;

        _rpmDataCharacteristic->setValue(rpmData);
        _rpmDataCharacteristic->notify();
        delay(100);
        _engineOilCharacteristic->setValue(eoilData);
        _engineOilCharacteristic->notify();
        delay(100);
        taskYIELD();
      }
      else
      {

        int configAsHex = SOFTWARE_VERSION_MAJOR << 24 | SOFTWARE_VERSION_MINOR << 20 | SOFTWARE_VERSION_BUGFIX << 16 | CDataPool::Instance()->GetData(SETTINGS_SHIFTLIGHTS_ENABLED) << 8 | CDataPool::Instance()->GetData(SETTINGS_SHIFTLIGHT_BRIGHTNESS);

        _configSenderCharacteristic->setValue(configAsHex);
        _configSenderCharacteristic->notify();
        delay(1000);
        taskYIELD();
      }
    }
    else
    {
      delay(500);
    }
    taskYIELD();
  }
}

void CBluetoothLETask::StartTaskImpl(void *_this)
{
  static_cast<CBluetoothLETask *>(_this)->Task();
}

void CBluetoothLETask::onWrite(BLECharacteristic *pCharacteristic)
{
  if (pCharacteristic->getUUID().equals(SHIFTER_ACTIVE_READER_UUID))
  {
    unsigned int appResponse = strtoul(pCharacteristic->getValue().c_str(), NULL, 10);
    int payload0 = appResponse & 0xFF;

    if (payload0 == 0)
    {
      _uiManager->SetShifterActive(false);
    }
    else if (payload0 == 1)
    {
      _uiManager->SetShifterActive(true);
    }
  }
  else if (pCharacteristic->getUUID().equals(SHIFTER_BRIGHTNESS_READER_UUID))
  {
    unsigned int appResponse = strtoul(pCharacteristic->getValue().c_str(), NULL, 10);
    int payload0 = appResponse & 0xFF;
    _uiManager->SetShifterBrightness(payload0);
  }
  else if (pCharacteristic->getUUID().equals(CONFIG_READER_CHARACTERISTIC_UUID))
  {
    unsigned int appResponse = strtoul(pCharacteristic->getValue().c_str(), NULL, 10);
    int payload0 = appResponse & 0xFF;
    int payload1 = (appResponse >> 8) & 0xFF;
    int payload2 = (appResponse >> 16) & 0xFF;

    if ((payload0 == 0xEF) && (payload1 == 0xEF) && (payload2 == 0xEF))
    {
      _deviceSuccessfullyReceivedConfig = true;
    }
  }
}

void CBluetoothLETask::onConnect(BLEServer *pServer)
{
  _deviceConnected = true;
}

void CBluetoothLETask::onDisconnect(BLEServer *pServer)
{
  _deviceConnected = false;
  _deviceSuccessfullyReceivedConfig = false;
}