#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "p2SString.h"
#include "p2Point.h"
#include "SDL\include\SDL.h"
#include "Animation.h"
#include "PugiXml\src\pugixml.hpp"
#include "p2DynArray.h"

class j1EntityManager;
struct Collider;

enum entity_state
{
	IDLE = 0,
	RIGHT,
	LEFT,
	JUMPING,
	FALLING,
	FLYING
};

enum class entity_type
{
	PLAYER,
};

class j1Entity
{
public:

	j1Entity(const char* entname,entity_type entitytype) : manager(NULL), entitytype(entitytype)
	{
		name.create(entname);
	}

	void Init(j1EntityManager* manager)
	{
		this->manager = manager;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
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

	virtual void OnCollision(Collider* c1, Collider* c2)
	{}

public:

	// --- Basic ---
	p2SString			name = nullptr;
	fPoint			position = {0,0};
	fPoint          Velocity = { 0,0 };

	// --- Collider data ---
	Collider*     entitycoll = nullptr;
	SDL_Rect entitycollrect = { 0,0,0,0 };
	float colliding_offset = 0;

	// --- Gravity ---
	float gravity = 0;

	// --- Entity ---
	entity_type  entitytype;
	entity_state entitystate;
	int entityID = 0;

	//--- Active or inactive ----
	bool active = false;

	// --- Animation ---
	Animation* CurrentAnimation = nullptr;

	// --- Spritesheet ---
	SDL_Texture* spritesheet = nullptr;

	j1EntityManager*	manager = nullptr;

};

#endif // __J1ENTITY_H__
