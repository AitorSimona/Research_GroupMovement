#include "j1Unit.h"

j1Unit::j1Unit(entity_info entityinfo, UnitInfo unitinfo):j1Entity(entity_type::UNIT,entityinfo)
{
}

void j1Unit::FixedUpdate(float dt)
{
}

void j1Unit::LogicUpdate(float dt)
{
}

void j1Unit::StateMachine()
{
	switch (state)
	{
	case UnitState::State_Idle:

		break;

	case UnitState::State_Moving:

		break;
	}
}

void j1Unit::SetUnitState(UnitState unitState)
{
	state = unitState;
}

UnitState j1Unit::GetUnitState() const
{
	return state;
}

// --- UnitInfo Constructors and Destructor ---
UnitInfo::UnitInfo(){}

UnitInfo::~UnitInfo(){}

UnitInfo::UnitInfo(const UnitInfo &info): color (info.color){}
