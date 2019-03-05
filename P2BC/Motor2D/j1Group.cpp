#include "j1Entity.h"
#include "j1Group.h"
#include "j1MovementManager.h"
#include "j1App.h"
#include "j1Input.h"

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

bool j1Group::SetGoal(iPoint goal)
{
	return false;
}

const iPoint j1Group::GetGoal()
{
	return goal;
}

void j1Group::ClearGroup()
{
	Units.clear();
}

int j1Group::GetSize()
{
	return Units.size();
}

bool j1Group::CheckForMovementRequest(float dt)
{
	if ((*Units.begin())->info.IsSelected)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			App->Mmanager->Move(this,dt);
	}

	return true;
}

