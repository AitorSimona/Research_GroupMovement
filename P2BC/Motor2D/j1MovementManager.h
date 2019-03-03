#ifndef __J1MOVEMENTMANAGER_H__
#define __J1MOVEMENTMANAGER_H__

#include "j1Module.h"
#include <list>
#include "j1Group.h"

class j1MovementManager : public j1Module
{
public:

	j1MovementManager();

	virtual ~j1MovementManager();

	// --- Called each loop iteration ---
	bool Update(float dt);

	// --- Called before quitting ---
	bool CleanUp();

private:

	std::list <j1Group*>	Groups;
};

#endif // __J1MOVEMENTMANAGER_H__
