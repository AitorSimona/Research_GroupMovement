#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "p2Point.h"
#include "j1MovementManager.h"
#include <vector>
#include "j1Group.h"

struct SDL_Texture;

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

	// --- Collider data ---
	iPoint Size = { 0,0 };
};


class j1Entity
{
public:

	j1Entity(entity_type entitytype, entity_info info) : entitytype(entitytype), Entityinfo(info)
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
	entity_info  Entityinfo;
	Group_Unit info;

	//--- Active or inactive ----
	bool active = false;

	// --- Spritesheet ---
	SDL_Texture* spritesheet = nullptr;
};


#endif // __J1ENTITY_H__
