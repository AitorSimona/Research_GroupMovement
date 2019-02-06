// ----------------------------------------------------
// Controls all logic entities (enemies/player/etc.) --
// ----------------------------------------------------

#ifndef __J1ENTITYMANAGER_H__
#define __J1ENTITYMANAGER_H__

#include "p2List.h"
#include "j1Module.h"
#include "j1Entity.h"
#include "j1Player.h"


class j1Entity;

struct PathInfo
{
	iPoint start_pos = { 0,0 };
	iPoint end_pos = { 0,0 };
	iPoint* path = nullptr;
	int path_size = 0;

	PathInfo();
	PathInfo(const PathInfo& i);
	~PathInfo() {
		RELEASE_ARRAY(path);
	}
};

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
	j1Entity * const CreateEntity(const char* entname , entity_type entitytype);
	void DestroyEntity(j1Entity* entity);
	void OnCollision(Collider* c1, Collider* c2);
	Animation* LoadAnimation(const char* animationPath, const char* animationName);

	// --- Get Entities data ---
	Playerdata& GetPlayerData() { return playerinfo; }

	// --- Save & Load ---
	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;
	
public:
	
	p2List <j1Entity*>	entities;
	bool				do_logic;
	int					logic_updates_per_second = 0;
	float				update_ms_cycle = 0;
	float				accumulated_time = 0;

	int					entityID = 0;

private:
	// --- Player ---
	Playerdata playerinfo;
};

#endif // __J1ENTITYMANAGER_H__
