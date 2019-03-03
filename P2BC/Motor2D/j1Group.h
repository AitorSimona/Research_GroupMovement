#ifndef __j1Group_H__
#define __j1Group_H__

#include "j1Entity.h"
#include "SDL\include\SDL.h"
#include <list>

class j1Group 
{
public:

	j1Group();
	~j1Group();

	// --- Add/Remove Units ---
	void addUnit(j1Entity* unit_toadd);
	void removeUnit(j1Entity* unit_toremove);

	// --- Goal ---
	bool SetGoal(iPoint goal);
    const iPoint GetGoal();

	// --- Clear Group ---
	void ClearGroup();

private:

	iPoint goal = { 0,0 };
	std::list <j1Entity*> Units;
};

#endif //__j1Group_H__
