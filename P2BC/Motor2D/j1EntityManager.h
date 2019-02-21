// ----------------------------------------------------
// Controls all logic entities (enemies/player/etc.) --
// ----------------------------------------------------

#ifndef __J1ENTITYMANAGER_H__
#define __J1ENTITYMANAGER_H__

#include "j1Module.h"
#include "j1Entity.h"
#include "j1Player.h"
#include <list>

class j1Entity;

//#define DEFAULT_LOGIC_PER_SECOND 60

class j1EntityManager : public j1Module
{
public:

	j1EntityManager();

	virtual ~j1EntityManager();

	// --- Called before render is available ---
	bool Awake(pugi::xml_node&);

	// --- Called before the first frame ---
	bool Start();

	// --- Called each loop iteration ---
	bool PreUpdate();
	bool Update(float dt);
	void UpdateEntity(float dt);
	bool PostUpdate(float dt);

	// --- Called before quitting ---
	bool CleanUp();

	// --- Entities management ---
	j1Entity * const CreateEntity(const char* entname, entity_type entitytype);

public:

	std::list <j1Entity*>	entities;
	bool				do_logic;
	int					logic_updates_per_second = 0;
	float				update_ms_cycle = 0;
	float				accumulated_time = 0;

};

#endif // __J1ENTITYMANAGER_H__