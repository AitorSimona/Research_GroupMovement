// ----------------------------------------------------
// Controls all logic entities (enemies/player/etc.) --
// ----------------------------------------------------

#ifndef __J1ENTITYMANAGER_H__
#define __J1ENTITYMANAGER_H__

#include "j1Module.h"
#include <list>
#include "j1Group.h"

class j1Entity;
enum class entity_type;
struct entity_info;
struct UnitInfo;
struct SDL_Texture;

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
	j1Entity * const CreateEntity(entity_type entitytype, entity_info entityinfo, UnitInfo unitinfo);

public:

	std::list <j1Entity*>	entities;
	float				update_ms_cycle = 0;
	SDL_Texture* sprite = nullptr;


private:

	j1Group Group;
};

#endif // __J1ENTITYMANAGER_H__