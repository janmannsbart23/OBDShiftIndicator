#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class ITask
{
public:
	virtual void StartTask() = 0;

private:
	virtual void Task() = 0;
};