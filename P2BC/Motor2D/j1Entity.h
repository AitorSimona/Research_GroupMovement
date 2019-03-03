#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "p2Point.h"

class j1EntityManager;
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

	j1Entity(entity_type entitytype, entity_info info) : manager(NULL), entitytype(entitytype), info(info)
	{
	}

	void Init(j1EntityManager* manager)
	{
		this->manager = manager;
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

	// --- Manager pointer ---
	j1EntityManager*	manager = nullptr;
};

#endif // __J1ENTITY_H__
