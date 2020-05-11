#ifndef LED_CONTROLLER_DISPLAY_H
#define LED_CONTROLLER_DISPLAY_H

#include "EPINDefines.h"
#include "FastLED.h"
#include <Arduino.h>
#include "ITask.hpp"

namespace eOperationMode
{
enum eOperationMode
{
    INIT = 0,
    WAITING_OIL_TEMP = 1,
    OIL_READY_ANIM = 2,
    ERROR_STATE = 3,
    SHIFTLIGHT = 4,
    OFF = 5
};
}

namespace eShifterType
{
enum eShifterType
{
    NORMAL = 13,
};
}

namespace eShiftMode
{
enum eShiftMode
{
    OFF = 0,
    MODE0 = 1,
    MODE1 = 2,
    MODE2 = 3
};
}

class CShiftLedTask : public ITask
{
private:
    CRGB *_leds;
    CRGB _initColor;
    CRGBPalette16 _initColorPalette;

    eShifterType::eShifterType _shifterType;
    eShiftMode::eShiftMode _shifterStartupMode;
    eOperationMode::eOperationMode _currentOperationMode;

    int _currentRPM;
    int _shiftLightRpmStepWidth;
    bool _shiftLightFlashToggle;
    bool _isBrightnessBeingChanged;

    void StateMachine();

    void Task();
    static void StartTaskImpl(void *_this);

public:
    CShiftLedTask(eShifterType::eShifterType shifterType, eShiftMode::eShiftMode shifterStartupMode);
    ~CShiftLedTask();

    void SetShifterOperationMode(eOperationMode::eOperationMode _newOperationMode);
    void SetShiftLightMode(eShiftMode::eShiftMode newMode);

    void NotifyNewShifterBrightnessIsSet();

    void StartTask();
};

#endif
