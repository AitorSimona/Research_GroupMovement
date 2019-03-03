#include "j1Group.h"

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
