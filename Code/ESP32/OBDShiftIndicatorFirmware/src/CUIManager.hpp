#ifndef C_UIMANAGER_HPP
#define C_UIMANAGER_HPP

#include "CShiftLedTask.hpp"

class CUIManager
{

public:
    CUIManager();
    virtual ~CUIManager();

    void SetShifterBrightness(char newShifterBrightness);
    void SetShifterActive(bool isActive);

private:
    CShiftLedTask *_shiftLedTask;
};


#endif