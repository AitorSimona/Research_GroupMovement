#ifndef __j1Group_H__
#define __j1Group_H__

#include <list>
#include "p2Point.h"

class j1Entity;


class j1Group 
{
public:
	friend class j1MovementManager;

	j1Group();
	~j1Group();

	// --- Add/Remove ---
	void addUnit(j1Entity* unit_toadd);
	void removeUnit(j1Entity* unit_toremove);
	void AddTiletoOccupied(iPoint to_add);

	// --- Clear Stuff ---
	void ClearGroup();
	void CleanOccupiedlist();

	// --- Getters ---
	int GetSize();

	// --- Check Movement Request ---
	void CheckForMovementRequest(float dt);

	// --- Enquiries ---
	bool IsGroupLead(j1Entity* entity);
	void SetUnitGoalTile(j1Entity* entity);
	bool FindFreeAdjacents(iPoint * base_tile);
	bool IsTileFree(iPoint* adjacent);

private:
	iPoint last_goal = { 0,0 };

	std::list <j1Entity*> Units;
	std::list <iPoint*> Occupied_tiles;
};

#endif //__j1Group_H__
