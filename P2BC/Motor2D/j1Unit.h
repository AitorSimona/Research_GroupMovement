#ifndef __Unit_H__
#define __Unit_H__

#include "j1Entity.h"
#include "SDL\include\SDL.h"

struct SDL_Color;

enum class UnitState {
	State_Idle,
	State_Moving
};

struct UnitInfo {

	UnitInfo();
	~UnitInfo();
	UnitInfo(const UnitInfo &info);

	SDL_Color color = { 255,255,255,255 };
};

class j1Unit : public j1Entity
{
public:

	j1Unit(entity_info entityinfo, UnitInfo unitinfo);

	// Called each loop iteration
	void FixedUpdate(float dt);

	// Called each logic iteration
	void LogicUpdate(float dt);

	void StateMachine();
	void SetUnitState(UnitState unitState);
	UnitState GetUnitState() const;

public:

	UnitInfo unitinfo;

private:

	UnitState state = UnitState::State_Idle;
};

#endif //__Entity_H__
