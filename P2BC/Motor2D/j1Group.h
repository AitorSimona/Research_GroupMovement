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

	// --- Add/Remove Units ---
	void addUnit(j1Entity* unit_toadd);
	void removeUnit(j1Entity* unit_toremove);

	// --- Goal ---
	void SetGoal(iPoint goal);
    const iPoint GetGoal();

	// --- Clear Group ---
	void ClearGroup();

	// --- Getters ---
	int GetSize();

	// --- Check Movement Request ---
	void CheckForMovementRequest(float dt);

	// --- Enquiries ---
	bool IsGroupLead(j1Entity* entity);


private:
	iPoint goal = { 0,0 };
	std::list <j1Entity*> Units;
};

#endif //__j1Group_H__
