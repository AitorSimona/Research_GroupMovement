#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "p2Point.h"
#include "j1MovementManager.h"
#include <vector>

struct SDL_Texture;
class j1Group;


enum class entity_type
{
	NONE,
	UNIT,
	MAX
};

struct entity_info
{
	// --- Basic ---
	fPoint			position = { 0,0 };
	int          Speed = 0;
	fPoint DirectionVector = { 0,0 };

	// --- Collider data ---
	iPoint Size = { 0,0 };

	bool IsSelected = false;

	// --- Path to follow ---
	std::vector <iPoint> Current_path;
	iPoint next_tile = { 0,0 };


};

class j1Entity
{
public:

	j1Entity(entity_type entitytype, entity_info info) : entitytype(entitytype), info(info)
	{
	}

	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual void FixedUpdate(float dt)
	{}

	// Called each logic iteration
	virtual void LogicUpdate(float dt)
	{}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

public:

	// --- Entity ---
	entity_type  entitytype;
	entity_info info;

	//--- Active or inactive ----
	bool active = false;

	// --- Spritesheet ---
	SDL_Texture* spritesheet = nullptr;

	// --- Group Movement stuff ---
	j1Group * current_group = nullptr;
	MovementState UnitMovementState = MovementState::MovementState_NoState;
};


#endif // __J1ENTITY_H__
