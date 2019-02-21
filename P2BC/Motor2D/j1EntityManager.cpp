// ----------------------------------------------------
// Controls all logic entities (enemies/player/etc.) --
// ----------------------------------------------------

#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"

j1EntityManager::j1EntityManager() : j1Module()
{
	name.assign("entities");
}

// Destructor
j1EntityManager::~j1EntityManager()
{}


// Called before render is available
bool j1EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Setting up Entity manager");
	bool ret = true;
	//logic_updates_per_second = DEFAULT_LOGIC_PER_SECOND;
	//update_ms_cycle = 1.0f / (float)logic_updates_per_second;
	update_ms_cycle = 1.0f / (float)App->framerate_cap;

	// ---------------------

	return ret;
}

// Called before the first frame
bool j1EntityManager::Start()
{
	LOG("start j1EntityManager");
	bool ret = true;
	return ret;
}

// Called each loop iteration
bool j1EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("EntityManager_Pre_Update", Profiler::Color::Chartreuse);

	//do_logic = false;
	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager_Update", Profiler::Color::Chocolate);

	//accumulated_time += dt;

	//if (accumulated_time >= update_ms_cycle)
	//{
	//	do_logic = true;
	//}

	if (dt<update_ms_cycle*1.25f && dt > 0.0f)
		UpdateEntity(dt);

	//if (do_logic == true)
	//{
	//	LOG("Did logic step after %f", accumulated_time);
	//	accumulated_time = 0.0f;
	//	do_logic = false;
	//}

	return true;
}

void j1EntityManager::UpdateEntity(float dt)
{
	std::list<j1Entity*>::iterator entity = entities.begin();

	while (entity != entities.end())
	{
		/*if (do_logic == true)
		{*/
		(*entity)->LogicUpdate(dt);
		//}

		++entity;
	}
}

bool j1EntityManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("EntityManager_Post_Update", Profiler::Color::Coral);

	std::list<j1Entity*>::iterator entity = entities.begin();

	while (entity != entities.end())
	{
		(*entity)->FixedUpdate(dt);

		++entity;
	}
	return true;
}

// Called before quitting
bool j1EntityManager::CleanUp()
{
	LOG("cleanup j1EntityManager");

	// release all entities
	std::list<j1Entity*>::iterator entity = entities.begin();

	while (entity != entities.end())
	{
		(*entity)->CleanUp();
		delete *entity;
		entity++;
	}

	entities.clear();
	return true;
}

// Create a new empty entity
j1Entity* const j1EntityManager::CreateEntity(const char* entname, entity_type entitytype)
{
	j1Entity* entity = nullptr;

	switch (entitytype)
	{
	case entity_type::PLAYER:
		entity = (j1Entity*) new j1Player();
		break;
	}

	entity->Init(this);
	entity->Start();
	entities.push_back(entity);
	return(entity);
}




