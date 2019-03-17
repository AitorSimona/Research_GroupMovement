#include "j1Entity.h"
#include "j1Group.h"
#include "j1MovementManager.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Pathfinding.h"

j1Group::j1Group()
{
}

j1Group::~j1Group()
{
}

void j1Group::addUnit(j1Entity * unit_toadd)
{
	Units.push_back(unit_toadd);
}

void j1Group::removeUnit(j1Entity * unit_toremove)
{
	Units.remove(unit_toremove);
}

void j1Group::AddTiletoOccupied(iPoint to_add)
{
	iPoint* new_tile = new iPoint;
	*new_tile = to_add;

	Occupied_tiles.push_back(new_tile);
}

void j1Group::ClearGroup()
{
	Units.clear();
}

void j1Group::ClearOccupiedlist()
{
	Occupied_tiles.clear();
}

int j1Group::GetSize()
{
	return Units.size();
}

void j1Group::CheckForMovementRequest(float dt)
{
 	App->Mmanager->Move(this,dt);
}

bool j1Group::IsGroupLead(j1Entity * entity)
{
	return (*Units.begin()) == entity;
}

void j1Group::SetUnitGoalTile(j1Entity* entity)
{
	std::list <j1Entity*>::const_iterator it = Units.begin();

	bool Goal_found = false;

	while (it != Units.end())
	{
		if (*it == entity)
		{
			it++;
			continue;
		}

		Goal_found = FindFreeAdjacents(&(*it)->info.goal_tile);

		if (Goal_found)
		{
			entity->info.goal_tile = last_goal;
			break;
		}

		it++;
	}

}

bool j1Group::FindFreeAdjacents(iPoint * base_tile)
{
	bool ret = false;

	// --- TODO 2: Check if any adjacent tile to the base_tile is free of entities ---

	iPoint cell;

	// --- TODO 2: Add a call to IsTileFree on each case ---

	// north
	cell.create(base_tile->x, base_tile->y + 1);
	if (App->pathfinding->IsWalkable(cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// north east
	cell.create(base_tile->x + 1, base_tile->y + 1);
	if (App->pathfinding->IsWalkable(cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// north west
	cell.create(base_tile->x - 1, base_tile->y + 1);
	if (App->pathfinding->IsWalkable(cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// south
	cell.create(base_tile->x, base_tile->y - 1);
	if (App->pathfinding->IsWalkable(cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// south east
	cell.create(base_tile->x + 1, base_tile->y - 1);
	if (App->pathfinding->IsWalkable(cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// south west
	cell.create(base_tile->x - 1, base_tile->y - 1);
	if (App->pathfinding->IsWalkable(cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// east
	cell.create(base_tile->x + 1, base_tile->y);
	if (App->pathfinding->IsWalkable(cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}

	// west
	cell.create(base_tile->x - 1, base_tile->y);
	if (App->pathfinding->IsWalkable(cell))
	{
		ret = true;
		last_goal = cell;
		return ret;
	}



	return ret;
}

bool j1Group::IsTileFree(iPoint* adjacent)
{
	std::list <iPoint*>::const_iterator it = Occupied_tiles.begin();

	// --- TODO 2: Search for the adjacent given in the occupied_tiles list ---


	// --- TODO 2: If the adjacent is found, return false --- 


	// --- TODO 2: If the adjacent is not found, add it to the occupied_tiles list ---

	return true;
}

