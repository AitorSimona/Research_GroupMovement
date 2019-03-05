#ifndef __j1Unit_H__
#define __j1Unit_H__

#include "SDL\include\SDL.h"
#include "j1Entity.h"

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

	// --- Draw circle to indicate SelectedUnits ---
	void DrawQuad();

public:

	UnitInfo unitinfo;

private:

	UnitState state = UnitState::State_Idle;
};

#endif //__j1Unit_H__
