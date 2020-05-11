#include "CShiftLedTask.hpp"
#include "CDataPool.hpp"
#include "DShiftModes.hpp"
#include "DConfig.hpp"
#include "EPINDefines.h"

#define SHIFTLIGHT_START_RPM 3000
#define SHIFLTIGHT_END_RPM 5000
#define SHIFTLIGHT_STEPS 6

#define STARTUP_ANIMATION_EYE_SIZE 2
#define STARTUP_ANIMATION_COLOR_R 0
#define STARTUP_ANIMATION_COLOR_G 0
#define STARTUP_ANIMATION_COLOR_B 255

#define OILTEMP_READY_ANIMATION_COLOR_R 0
#define OILTEMP_READY_ANIMATION_COLOR_G 255
#define OILTEMP_READY_ANIMATION_COLOR_B 0

CRGB INIT_COLOR = CRGB(15, 15, 255);

CShiftLedTask::CShiftLedTask(eShifterType::eShifterType shifterType, eShiftMode::eShiftMode shifterStartupMode)
{
    _isBrightnessBeingChanged = false;
    _shifterType = shifterType;
    _shifterStartupMode = shifterStartupMode;

    _currentOperationMode = eOperationMode::INIT;

    _shiftLightRpmStepWidth = (SHIFLTIGHT_END_RPM - SHIFTLIGHT_START_RPM) / SHIFTLIGHT_STEPS;
    _shiftLightFlashToggle = false;

    _leds = new CRGB[shifterType];

    FastLED.addLeds<WS2812B, ePIN::DIGITAL_LEDS, RGB>(_leds, shifterType).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(CDataPool::Instance()->GetData(SETTINGS_SHIFTLIGHT_BRIGHTNESS));
}

CShiftLedTask::~CShiftLedTask()
{
}

void CShiftLedTask::SetShifterOperationMode(eOperationMode::eOperationMode _newOperationMode)
{
}
void CShiftLedTask::SetShiftLightMode(eShiftMode::eShiftMode newMode)
{
}

void CShiftLedTask::StartTask()
{
    xTaskCreatePinnedToCore(this->StartTaskImpl, "Shiftligth Task", 2048, this, SHIFT_LIGHT_TASK_PRIO, NULL, SHIFT_LIGHT_CORE);
}

void CShiftLedTask::StartTaskImpl(void *_this)
{
    static_cast<CShiftLedTask *>(_this)->Task();
}

void CShiftLedTask::Task()
{

    for (int i = 0; i < _shifterType; i++)
    {
        _leds[i] = CRGB::Black;
    }
    FastLED.show();

    while (1)
    {
        if (!_isBrightnessBeingChanged)
        {
            StateMachine();
        }
        delay(40);
    }
}

void CShiftLedTask::StateMachine()
{
    if (CDataPool::Instance()->GetData(SETTINGS_SHIFTLIGHTS_ENABLED) == 1)
    {
        if (CDataPool::Instance()->GetData(OBD_RPM) <= 500)
        {
            //Engine OFF -> reset state machine
            _currentOperationMode = eOperationMode::INIT;
        }
        switch (_currentOperationMode)
        {
        case eOperationMode::INIT:
            if (CDataPool::Instance()->GetData(OBD_RPM) > 500)
            {
                //Show startup animation
                for (int animLoop = 0; animLoop < 5; animLoop++)
                {
                    for (int i = 0; i <= ((_shifterType - STARTUP_ANIMATION_EYE_SIZE) / 2); i++)
                    {
                        for (int i = 0; i < _shifterType; i++)
                        {
                            _leds[i] = CRGB::Black;
                        }

                        _leds[i] = CRGB(STARTUP_ANIMATION_COLOR_R / 10, STARTUP_ANIMATION_COLOR_G / 10, STARTUP_ANIMATION_COLOR_B / 10);

                        for (int j = 1; j <= STARTUP_ANIMATION_EYE_SIZE; j++)
                        {
                            _leds[i + j] = CRGB(STARTUP_ANIMATION_COLOR_R, STARTUP_ANIMATION_COLOR_G, STARTUP_ANIMATION_COLOR_B);
                        }

                        _leds[i + STARTUP_ANIMATION_EYE_SIZE + 1] = CRGB(STARTUP_ANIMATION_COLOR_R / 10, STARTUP_ANIMATION_COLOR_G / 10, STARTUP_ANIMATION_COLOR_B / 10);

                        _leds[_shifterType - i] = CRGB(STARTUP_ANIMATION_COLOR_R / 10, STARTUP_ANIMATION_COLOR_G / 10, STARTUP_ANIMATION_COLOR_B / 10);

                        for (int j = 1; j <= STARTUP_ANIMATION_EYE_SIZE; j++)
                        {
                            _leds[_shifterType - i - j] = CRGB(STARTUP_ANIMATION_COLOR_R, STARTUP_ANIMATION_COLOR_G, STARTUP_ANIMATION_COLOR_B);
                        }
                        _leds[_shifterType - i - STARTUP_ANIMATION_EYE_SIZE - 1] = CRGB(STARTUP_ANIMATION_COLOR_R / 10, STARTUP_ANIMATION_COLOR_G / 10, STARTUP_ANIMATION_COLOR_B / 10);

                        FastLED.show();
                        delay(30);
                    }

                    for (int i = ((_shifterType - STARTUP_ANIMATION_EYE_SIZE) / 2); i >= 0; i--)
                    {
                        for (int i = 0; i < _shifterType; i++)
                        {
                            _leds[i] = CRGB::Black;
                        }

                        _leds[i] = CRGB(STARTUP_ANIMATION_COLOR_R / 10, STARTUP_ANIMATION_COLOR_G / 10, STARTUP_ANIMATION_COLOR_B / 10);

                        for (int j = 1; j <= STARTUP_ANIMATION_EYE_SIZE; j++)
                        {
                            _leds[i + j] = CRGB(STARTUP_ANIMATION_COLOR_R, STARTUP_ANIMATION_COLOR_G, STARTUP_ANIMATION_COLOR_B);
                        }
                        _leds[i + STARTUP_ANIMATION_EYE_SIZE + 1] = CRGB(STARTUP_ANIMATION_COLOR_R / 10, STARTUP_ANIMATION_COLOR_G / 10, STARTUP_ANIMATION_COLOR_B / 10);
                        _leds[_shifterType - i] = CRGB(STARTUP_ANIMATION_COLOR_R / 10, STARTUP_ANIMATION_COLOR_G / 10, STARTUP_ANIMATION_COLOR_B / 10);

                        for (int j = 1; j <= STARTUP_ANIMATION_EYE_SIZE; j++)
                        {
                            _leds[_shifterType - i - j] = CRGB(STARTUP_ANIMATION_COLOR_R, STARTUP_ANIMATION_COLOR_G, STARTUP_ANIMATION_COLOR_B);
                        }
                        _leds[_shifterType - i - STARTUP_ANIMATION_EYE_SIZE - 1] = CRGB(STARTUP_ANIMATION_COLOR_R / 10, STARTUP_ANIMATION_COLOR_G / 10, STARTUP_ANIMATION_COLOR_B / 10);

                        FastLED.show();
                        delay(30);
                    }
                }

                for (int maxBright = CDataPool::Instance()->GetData(SETTINGS_SHIFTLIGHT_BRIGHTNESS); maxBright >= 0; maxBright--)
                {
                    FastLED.setBrightness(maxBright);
                    FastLED.show();
                    delay(2);
                }

                for (int i = 0; i < _shifterType; i++)
                {
                    _leds[i] = CRGB::Black;
                }
                FastLED.show();
                FastLED.setBrightness(CDataPool::Instance()->GetData(SETTINGS_SHIFTLIGHT_BRIGHTNESS));
                _currentOperationMode = eOperationMode::WAITING_OIL_TEMP;
            }

            break;
        case eOperationMode::WAITING_OIL_TEMP:
            if (CDataPool::Instance()->GetData(OBD_ENGINE_OIL_TEMP) >= 90)
            {
                _currentOperationMode = eOperationMode::OIL_READY_ANIM;
            }
            break;
        case eOperationMode::OIL_READY_ANIM:

            for (int j = 0; j < 10; j++)
            { //do 10 cycles of chasing
                for (int q = 0; q < 3; q++)
                {
                    for (int i = 0; i < _shifterType; i = i + 3)
                    {
                        _leds[i + q] = CRGB(OILTEMP_READY_ANIMATION_COLOR_R, OILTEMP_READY_ANIMATION_COLOR_G, OILTEMP_READY_ANIMATION_COLOR_B);
                    }
                    FastLED.show();

                    delay(75);

                    for (int i = 0; i < _shifterType; i = i + 3)
                    {
                        _leds[i + q] = CRGB(0, 0, 0);
                    }
                }
            }

            _currentOperationMode = eOperationMode::SHIFTLIGHT;
            break;
        case eOperationMode::ERROR_STATE:
            //leds red
            break;
        case eOperationMode::SHIFTLIGHT:
            _currentRPM = CDataPool::Instance()->GetData(OBD_RPM);
            if (_currentRPM < SHIFTLIGHT_START_RPM)
            {
                //leds off
                for (int i = 0; i < _shifterType; i++)
                {
                    _leds[i] = shiftLightMode1[0][i];
                }
            }
            else if (_currentRPM >= SHIFTLIGHT_START_RPM + 0 * _shiftLightRpmStepWidth && _currentRPM < SHIFTLIGHT_START_RPM + 1 * _shiftLightRpmStepWidth)
            {
                //led 1 - 13
                for (int i = 0; i < _shifterType; i++)
                {
                    _leds[i] = shiftLightMode1[1][i];
                }
            }
            else if (_currentRPM >= SHIFTLIGHT_START_RPM + 1 * _shiftLightRpmStepWidth && _currentRPM < SHIFTLIGHT_START_RPM + 2 * _shiftLightRpmStepWidth)
            {
                //led 2 - 12
                for (int i = 0; i < _shifterType; i++)
                {
                    _leds[i] = shiftLightMode1[2][i];
                }
            }
            else if (_currentRPM >= SHIFTLIGHT_START_RPM + 2 * _shiftLightRpmStepWidth && _currentRPM < SHIFTLIGHT_START_RPM + 3 * _shiftLightRpmStepWidth)
            {
                //led 3 - 11
                for (int i = 0; i < _shifterType; i++)
                {
                    _leds[i] = shiftLightMode1[3][i];
                }
            }
            else if (_currentRPM >= SHIFTLIGHT_START_RPM + 3 * _shiftLightRpmStepWidth && _currentRPM < SHIFTLIGHT_START_RPM + 4 * _shiftLightRpmStepWidth)
            {
                //led 4 - 10
                for (int i = 0; i < _shifterType; i++)
                {
                    _leds[i] = shiftLightMode1[4][i];
                }
            }
            else if (_currentRPM >= SHIFTLIGHT_START_RPM + 4 * _shiftLightRpmStepWidth && _currentRPM < SHIFTLIGHT_START_RPM + 5 * _shiftLightRpmStepWidth)
            {
                //led 5 - 9
                for (int i = 0; i < _shifterType; i++)
                {
                    _leds[i] = shiftLightMode1[5][i];
                }
            }
            else if (_currentRPM >= SHIFTLIGHT_START_RPM + 5 * _shiftLightRpmStepWidth && _currentRPM < SHIFTLIGHT_START_RPM + 6 * _shiftLightRpmStepWidth)
            {
                //led 6 - 8
                for (int i = 0; i < _shifterType; i++)
                {
                    _leds[i] = shiftLightMode1[6][i];
                }
            }
            else if (_currentRPM >= SHIFTLIGHT_START_RPM + 6 * _shiftLightRpmStepWidth)
            {
                //led   7
                if (_shiftLightFlashToggle)
                {
                    _shiftLightFlashToggle = false;
                    for (int i = 0; i < _shifterType; i++)
                    {
                        _leds[i] = shiftLightMode1[7][i];
                    }
                    //leds on
                }
                else
                {
                    _shiftLightFlashToggle = true;
                    for (int i = 0; i < _shifterType; i++)
                    {
                        _leds[i] = shiftLightMode1[0][i];
                    }
                    //leds black
                }
            }
            break;
        case eOperationMode::OFF:
            _leds = shiftLightMode1[0];
            break;
        }
    }
    else
    {
        for (int i = 0; i < _shifterType; i++)
        {
            _leds[i] = CRGB::Black;
        }
    }

    FastLED.show();
}

void CShiftLedTask::NotifyNewShifterBrightnessIsSet()
{
    _isBrightnessBeingChanged = true;
    FastLED.setBrightness(CDataPool::Instance()->GetData(SETTINGS_SHIFTLIGHT_BRIGHTNESS));
    for (int i = 0; i < _shifterType; i++)
    {
        _leds[i] = shiftLightMode1[7][i];
    }
    FastLED.show();
    delay(2000);
    for (int i = 0; i < _shifterType; i++)
    {
        _leds[i] = CRGB::Black;
    }
    FastLED.show();
    _isBrightnessBeingChanged = false;
}