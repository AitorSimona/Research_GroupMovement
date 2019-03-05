#include "j1Unit.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1EntityManager.h"
#include "j1Textures.h"

j1Unit::j1Unit(entity_info entityinfo, UnitInfo unitinfo):j1Entity(entity_type::UNIT,entityinfo)
{
}

void j1Unit::FixedUpdate(float dt)
{
	const SDL_Rect unit_rect { 0,0, info.Size.x, info.Size.y };

	if (info.IsSelected)
		DrawQuad();

	App->render->Blit(App->manager->sprite, info.position.x, info.position.y, &unit_rect);
}

void j1Unit::LogicUpdate(float dt)
{
}

void j1Unit::StateMachine()
{
	switch (state)
	{
	case UnitState::State_Idle:

		break;

	case UnitState::State_Moving:

		break;
	}
}

void j1Unit::SetUnitState(UnitState unitState)
{
	state = unitState;
}

UnitState j1Unit::GetUnitState() const
{
	return state;
}

void j1Unit::DrawQuad()
{
	const SDL_Rect entityrect = { info.position.x,  info.position.y,  info.Size.x,  info.Size.y };
	App->render->DrawQuad(entityrect, unitinfo.color.r, unitinfo.color.g, unitinfo.color.b, unitinfo.color.a,false);
}

// --- UnitInfo Constructors and Destructor ---
UnitInfo::UnitInfo(){}

UnitInfo::~UnitInfo(){}

UnitInfo::UnitInfo(const UnitInfo &info): color (info.color){}
