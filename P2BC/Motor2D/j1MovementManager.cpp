#include "j1Entity.h"
#include "j1Group.h"
#include "j1MovementManager.h"
#include "j1App.h"
#include "j1EntityManager.h" 
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Input.h"
#include "Brofiler\Brofiler.h"

j1MovementManager::j1MovementManager()
{
}

j1MovementManager::~j1MovementManager()
{
}

bool j1MovementManager::Update(float dt)
{
	return true;
}

bool j1MovementManager::CleanUp()
{
	std::list <j1Group*>::const_iterator group = Groups.begin();

	while (group != Groups.end())
	{
		(*group)->ClearGroup();
		delete *group;
		group++;
	}

	Groups.clear();

	return true;
}

void j1MovementManager::SelectEntities_inRect(SDL_Rect SRect)
{
	BROFILER_CATEGORY("GroupMovement::SelectEntities_inRect", Profiler::Color::DarkOliveGreen);

	std::list<j1Entity*>::iterator entity = App->manager->entities.begin();
	SDL_Rect entityrect = { 0,0,0,0 };

	while (entity != App->manager->entities.end())
	{
		entityrect = { (int)(*entity)->Entityinfo.position.x, (int)(*entity)->Entityinfo.position.y, (*entity)->Entityinfo.Size.x, (*entity)->Entityinfo.Size.y };

		// --- Check entity's rect against the given SRect, select it if overlap is positive ---
		if (SDL_HasIntersection(&entityrect, &SRect))
			(*entity)->info.IsSelected = true;
		else
			(*entity)->info.IsSelected = false;

		entity++;
	}

}

void j1MovementManager::CreateGroup()
{
	BROFILER_CATEGORY("GroupMovement::CreateGroup", Profiler::Color::GhostWhite);

	bool Validgroup = false;

	j1Group* group = new j1Group;

	std::list<j1Entity*>::iterator entity = App->manager->entities.begin();

	while (entity != App->manager->entities.end())
	{

		// --- Check for entities that have been selected ---
		if ((*entity)->info.IsSelected)
		{
			// --- If we find an entity then the group is valid and can be created ---
			Validgroup = true;

			// --- Remove the entity from a previous group if any is found ---
			if ((*entity)->info.current_group != nullptr)
			{
				(*entity)->info.current_group->removeUnit(*entity);

				if ((*entity)->info.current_group->GetSize() == 0)
					Groups.remove((*entity)->info.current_group);
			}

			// --- Add the entity to the new group, update its current group pointer ---
			group->addUnit(*entity);
			(*entity)->info.current_group = group;
		}

		entity++;
	}

	// --- Finally, If the group is Valid add it to our Groups list, else delete it ---
	if (Validgroup)
		Groups.push_back(group);
	else
		delete group;

    //LOG("Groups size %i", Groups.size());
}

void j1MovementManager::Move(j1Group * group, float dt)
{
	BROFILER_CATEGORY("GroupMovement::Move", Profiler::Color::Gold);

	std::list <j1Entity*>::const_iterator unit = group->Units.begin();

	LOG("On Move Function");

	iPoint Map_Entityposition;
	fPoint distanceToNextTile;
	iPoint next_tile_world;
	float DirectDistance;
	fPoint to_fPoint;

	// --- We get the map coords of the mouse ---
	iPoint Map_mouseposition;
	Map_mouseposition = App->map->WorldToMap((int)App->scene->mouse_pos.x, (int)App->scene->mouse_pos.y);


	while (unit != group->Units.end())
	{
		// --- We Get the map coords of the Entity ---
		Map_Entityposition.x = (*unit)->Entityinfo.position.x;
		Map_Entityposition.y = (*unit)->Entityinfo.position.y;
		Map_Entityposition = App->map->WorldToMap(Map_Entityposition.x, Map_Entityposition.y);


		switch ((*unit)->info.UnitMovementState)
		{

		case MovementState::MovementState_NoState:

			// --- On call to Move, Units will request a path to the destination ---

			if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && (*unit)->info.IsSelected)
			{
				
				if (group->IsGroupLead((*unit)) == false)
				{
					// --- If any other unit of the group has the same goal, change the goal tile ---
					group->SetUnitGoalTile((*unit));
				}
				else
				{	
					// --- Clear previous path request occupied goal tiles ---
					group->ClearOccupiedlist();
					(*unit)->info.goal_tile = Map_mouseposition;
					group->Occupied_tiles.push_back(&(*unit)->info.goal_tile);
				}

				if (App->pathfinding->CreatePath(Map_Entityposition, (*unit)->info.goal_tile) != -1)
				{
					(*unit)->info.Current_path = *App->pathfinding->GetLastPath();
					(*unit)->info.Current_path.erase((*unit)->info.Current_path.begin());
					(*unit)->info.Current_path.erase((*unit)->info.Current_path.begin());

					(*unit)->info.UnitMovementState = MovementState::MovementState_NextStep;
				}
				else
					stop_iteration = true;
			}

			break;

		case MovementState::MovementState_Wait:

			// --- Whenever the unit faces an obstacle of any type during a scheduled path, overcome it ---

			break;

		case MovementState::MovementState_FollowPath:

			// --- If a path is created, the unit will start following it ---

			next_tile_world = App->map->MapToWorld((*unit)->info.next_tile.x, (*unit)->info.next_tile.y);

			distanceToNextTile = { (float)next_tile_world.x - (*unit)->Entityinfo.position.x,(float)next_tile_world.y - (*unit)->Entityinfo.position.y };

			DirectDistance = sqrtf(pow(distanceToNextTile.x, 2.0f) + pow(distanceToNextTile.y, 2.0f));

			//LOG("Next tile pos : x = %i y= %i", next_tile_world.x, next_tile_world.y);

			// --- We want a vector to update the unit's direction/position ---
			if (DirectDistance > 0.0f)
			{
				distanceToNextTile.x /= DirectDistance;
				distanceToNextTile.y /= DirectDistance;
			}

			// --- Now we Apply the unit's Speed and the dt to the Distance we need to advance  ---
			distanceToNextTile.x *= (*unit)->Entityinfo.Speed*dt;
			distanceToNextTile.y *= (*unit)->Entityinfo.Speed*dt;

			// --- We convert an iPoint to fPoint for comparing purposes ---
			to_fPoint.x = next_tile_world.x;
			to_fPoint.y = next_tile_world.y;

			if ((*unit)->Entityinfo.position.DistanceTo(to_fPoint) < 3)
			{
				(*unit)->Entityinfo.position.x = next_tile_world.x;
				(*unit)->Entityinfo.position.y = next_tile_world.y;

				(*unit)->info.UnitMovementState = MovementState::MovementState_NextStep;
			}

			else
			{
				(*unit)->Entityinfo.position.x += distanceToNextTile.x;
				(*unit)->Entityinfo.position.y += distanceToNextTile.y;
			}

			break;

		case MovementState::MovementState_NextStep:

			// --- If a path is being followed, the unit will get the next tile in the path ---

			if ((*unit)->info.Current_path.size() > 0)
			{
				(*unit)->info.next_tile = (*unit)->info.Current_path.front();
				(*unit)->info.Current_path.erase((*unit)->info.Current_path.begin());

				(*unit)->info.UnitMovementState = MovementState::MovementState_FollowPath;
			}
			else
			{
				(*unit)->info.UnitMovementState = MovementState::MovementState_DestinationReached;
			}

			break;

		case MovementState::MovementState_DestinationReached:

			// --- The unit reaches the end of the path, thus stopping and returning to NoState ---

			(*unit)->info.UnitMovementState = MovementState::MovementState_NoState;

			break;
		}


		if (stop_iteration)
		{
			stop_iteration = false;
			break;
		}

		unit++;

	}

}

