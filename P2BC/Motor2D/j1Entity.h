#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "p2Point.h"
#include "SDL\include\SDL.h"
#include "PugiXml\src\pugixml.hpp"
#include <string>

class j1EntityManager;

enum class entity_type
{
	PLAYER,
};

class j1Entity
{
public:

	j1Entity(const char* entname, entity_type entitytype) : manager(NULL), entitytype(entitytype)
	{
		name.assign(entname);
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

public:

	// --- Basic ---
	std::string			name;
	fPoint			position = { 0,0 };
	fPoint          Velocity = { 0,0 };

	// --- Collider data ---
	SDL_Rect entitycollrect = { 0,0,0,0 };

	// --- Entity ---
	entity_type  entitytype;

	//--- Active or inactive ----
	bool active = false;

	// --- Spritesheet ---
	SDL_Texture* spritesheet = nullptr;

	j1EntityManager*	manager = nullptr;
};

#endif // __J1ENTITY_H__
