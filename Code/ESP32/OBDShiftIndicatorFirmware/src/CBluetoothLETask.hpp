#pragma once

#include "ITask.hpp"
#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "CUIManager.hpp"

#define SHIFTINDICATOR_SERVICE BLEUUID((uint16_t)0xBBB0)
#define SHIFTINDICATOR_SERVICE2 BLEUUID((uint16_t)0xBBB1)

#define RPM_CHARACTERISTIC_UUID BLEUUID((uint16_t)0xAAA0)
#define EOIL_CHARACTERISTIC_UUID BLEUUID((uint16_t)0xAAA1)

#define SHIFTER_ACTIVE_READER_UUID BLEUUID((uint16_t)0xAAA0)
#define SHIFTER_BRIGHTNESS_READER_UUID BLEUUID((uint16_t)0xAAA1)

#define CONFIG_SENDER_CHARACTERISTIC_UUID BLEUUID((uint16_t)0xAAA2)
#define CONFIG_READER_CHARACTERISTIC_UUID BLEUUID((uint16_t)0xAAA3)

class CBluetoothLETask : public ITask, public BLEServerCallbacks, public BLECharacteristicCallbacks
{

public:
    CBluetoothLETask(CUIManager *uiManager);
    virtual ~CBluetoothLETask();

    void StartTask();

private:
    bool _deviceConnected;
    bool _deviceSuccessfullyReceivedConfig;

    CUIManager *_uiManager;

    BLEServer *_pServer;
    BLEService *_pService;
     BLEService *_pService2;

    BLECharacteristic *_rpmDataCharacteristic;
    BLECharacteristic *_engineOilCharacteristic;
    BLECharacteristic *_configSenderCharacteristic;

    BLECharacteristic *_shifterActiveReaderCharacteristic;
    BLECharacteristic *_shifterBrightnessReaderCharacteristic;

    BLECharacteristic *_configReadCharacteristic;

    void onWrite(BLECharacteristic *pCharacteristic);
    void onConnect(BLEServer *pServer);
    void onDisconnect(BLEServer *pServer);

    void Task();
    static void StartTaskImpl(void *_this);
};