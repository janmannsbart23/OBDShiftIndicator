#include "ITask.hpp"
#include <ESP32CAN.h>
#include <CAN_config.h>
#include "ICanEventCallback.hpp"

#define MODE_01 0x01 //Show current data
#define MODE_02 0x02 //Show freezeframe data
#define MODE_RESPONSE_OFFSET 0x40

#define PID_RPM 0x0C
#define PID_SPEED 0x0D
#define PID_ENINGE_LOAD 0x04
#define PID_INTAKE_AIR_TEMP 0x0F
#define PID_THROTTLE_POSITION 0x11
#define PID_ENGINE_OIL_TEMP 0x5C
#define PID_COOLANT_TEMP 0x05
#define PID_ENGINE_COOLANT_TEMP 0x67
#define PID_CURRENT_GEAR 0xA4
#define PID_FUEL_TANK_LEVEL 0x2F

#define CAN_ID_OBD_REQUEST 0x7DF
#define CAN_ID_OBD_RESPONSE 0x7E8

class CCanTask : public ITask
{

public:
    CCanTask(ICanEventCallback *canEventCallback);
    virtual ~CCanTask();

    void StartTask();

    void SendOBDPID(int pidByteLength, int mode, char pid);

private:
    CAN_frame_t _rxMsg;

    unsigned long _previousMillis;

    ICanEventCallback *_canEventCallback;

    void Task();
    static void StartTaskImpl(void *_this);
};
