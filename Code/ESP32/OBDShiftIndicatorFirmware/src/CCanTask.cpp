#include "CCanTask.hpp"
#include "CDataPool.hpp"
#include "DConfig.hpp"
#include <Arduino.h>
#include "EPINDefines.h"
CAN_device_t CAN_cfg;

CCanTask::CCanTask(ICanEventCallback *canEventCallback)
{
    _canEventCallback = canEventCallback;
    //Setup NM
    pinMode(ePIN::CAN_EN, OUTPUT);
    pinMode(ePIN::CAN_NSTB, OUTPUT);

    digitalWrite(ePIN::CAN_EN, HIGH);
    digitalWrite(ePIN::CAN_NSTB, HIGH);

    CAN_cfg.speed = CAN_SPEED_500KBPS;
    CAN_cfg.tx_pin_id = (gpio_num_t)ePIN::CAN_TX;
    CAN_cfg.rx_pin_id = (gpio_num_t)ePIN::CAN_RX;
    CAN_cfg.rx_queue = xQueueCreate(CAN_RX_QUEUE_SIZE, sizeof(CAN_frame_t));

    // Init CAN Module
    ESP32Can.CANInit();
}

CCanTask::~CCanTask()
{
}

void CCanTask::StartTask()
{
    xTaskCreatePinnedToCore(this->StartTaskImpl, "CANTask", 4096, this, CAN_TASK_PRIO, NULL, CAN_CORE);
}

void CCanTask::Task()
{
    _previousMillis = millis();

    while (1)
    {

        if (xQueueReceive(CAN_cfg.rx_queue, &_rxMsg, 2 * portTICK_PERIOD_MS) == pdTRUE)
        {
            _previousMillis = millis();

            if (_rxMsg.FIR.B.RTR != CAN_RTR)
            {
                switch (_rxMsg.MsgID)
                {
                case CAN_ID_OBD_RESPONSE:

                    int pid = _rxMsg.data.u8[2];

                    int obdPayload;

                    switch (pid)
                    {
                    case PID_RPM:
                        obdPayload = ((_rxMsg.data.u8[3] << 8) + _rxMsg.data.u8[4]) / 4;
                        CDataPool::Instance()->SetData(OBD_RPM, obdPayload);
                        _canEventCallback->NewCanDataEvent(eNewCanDataEvent::EVENT_RPM);
                        break;
                    case PID_SPEED:
                        obdPayload = _rxMsg.data.u8[3];
                        CDataPool::Instance()->SetData(OBD_SPEED, obdPayload);
                        _canEventCallback->NewCanDataEvent(eNewCanDataEvent::EVENT_SPEED);
                        break;
                    case PID_ENINGE_LOAD:
                        obdPayload = _rxMsg.data.u8[3] * 100 / 255;
                        CDataPool::Instance()->SetData(OBD_ENGINE_LOAD, obdPayload);
                        _canEventCallback->NewCanDataEvent(eNewCanDataEvent::EVENT_ENGINE_LOAD);
                        break;
                    case PID_THROTTLE_POSITION:
                        obdPayload = _rxMsg.data.u8[3] * 100 / 255;
                        CDataPool::Instance()->SetData(OBD_THROTTLE_POSITION, obdPayload);
                        _canEventCallback->NewCanDataEvent(eNewCanDataEvent::EVENT_THROTTLE_POSITION);
                        break;
                    case PID_INTAKE_AIR_TEMP:
                        obdPayload = _rxMsg.data.u8[3] - 40;
                        CDataPool::Instance()->SetData(OBD_INTAKE_AIR_TEMP, obdPayload);
                        _canEventCallback->NewCanDataEvent(eNewCanDataEvent::EVENT_INTAKE_AIR_TEMP);
                        break;
                    case PID_ENGINE_OIL_TEMP:
                        obdPayload = _rxMsg.data.u8[3] - 40;
                        CDataPool::Instance()->SetData(OBD_ENGINE_OIL_TEMP, obdPayload);
                        _canEventCallback->NewCanDataEvent(eNewCanDataEvent::EVENT_ENGINE_OIL_TEMP);
                        break;
                    case PID_COOLANT_TEMP:
                        obdPayload = _rxMsg.data.u8[3] - 40;
                        CDataPool::Instance()->SetData(OBD_COOLANT_TEMP, obdPayload);
                        _canEventCallback->NewCanDataEvent(eNewCanDataEvent::EVENT_COOLANT_TEMP);
                        break;
                    case PID_CURRENT_GEAR:
                        obdPayload = _rxMsg.data.u8[3];
                        CDataPool::Instance()->SetData(OBD_GEAR, obdPayload);
                        _canEventCallback->NewCanDataEvent(eNewCanDataEvent::EVENT_CURRENT_GEAR);
                        break;
                    case PID_FUEL_TANK_LEVEL:
                        obdPayload = _rxMsg.data.u8[3] * 100 / 255;
                        CDataPool::Instance()->SetData(OBD_FUEL_LEVEL, obdPayload);
                        _canEventCallback->NewCanDataEvent(eNewCanDataEvent::EVENT_FUEL_TANK_LEVEL);
                        break;
                    }
                    break;
                }
            }
        }
        if ((millis() - _previousMillis) >= 2000)
        {
            //SHUTDOWN
            if (IS_SYSTEM_RUHESTROMSICHER)
            {
                digitalWrite(ePIN::CAN_EN, HIGH);
                digitalWrite(ePIN::CAN_NSTB, LOW);
            }
        }
    }
}

void CCanTask::StartTaskImpl(void *_this)
{
    static_cast<CCanTask *>(_this)->Task();
}

void CCanTask::SendOBDPID(int pidByteLength, int mode, char pid)
{
    CAN_frame_t obdFrame;
    obdFrame.FIR.B.RTR = CAN_no_RTR;
    obdFrame.FIR.B.FF = CAN_frame_std;
    obdFrame.MsgID = CAN_ID_OBD_REQUEST;
    obdFrame.FIR.B.DLC = 8;
    obdFrame.data.u8[0] = pidByteLength + 1;
    obdFrame.data.u8[1] = mode;
    obdFrame.data.u8[2] = pid;
    obdFrame.data.u8[3] = 0xFF;
    obdFrame.data.u8[4] = 0xFF;
    obdFrame.data.u8[5] = 0xFF;
    obdFrame.data.u8[6] = 0xFF;
    obdFrame.data.u8[7] = 0xFF;

    ESP32Can.CANWriteFrame(&obdFrame);
}
