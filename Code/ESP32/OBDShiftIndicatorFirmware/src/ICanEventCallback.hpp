#pragma once

namespace eNewCanDataEvent
{
enum eNewCanDataEvent
{
	EVENT_RPM,
	EVENT_SPEED,
	EVENT_ENGINE_LOAD,
	EVENT_INTAKE_AIR_TEMP,
	EVENT_THROTTLE_POSITION,
	EVENT_ENGINE_OIL_TEMP,
	EVENT_COOLANT_TEMP,
	EVENT_CURRENT_GEAR,
	EVENT_FUEL_TANK_LEVEL,
	NO_EVENT = 0xFF

};
}

class ICanEventCallback
{
public:
	virtual void NewCanDataEvent(eNewCanDataEvent::eNewCanDataEvent newCanDataEventType) = 0;
};