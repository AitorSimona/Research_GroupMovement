#include "j1Unit.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1EntityManager.h"
#include "j1Textures.h"
#include "j1Group.h"
#include "j1Map.h"
#include "j1Scene.h"

j1Unit::j1Unit(entity_info entityinfo, UnitInfo unitinfo):j1Entity(entity_type::UNIT,entityinfo)
{
}

void j1Unit::FixedUpdate(float dt)
{
	const SDL_Rect unit_rect { 0,0, Entityinfo.Size.x, Entityinfo.Size.y };

	if (info.IsSelected)
		DrawQuad();

	App->render->Blit(App->manager->sprite, Entityinfo.position.x + App->map->data.tile_width / 3, Entityinfo.position.y + App->map->data.tile_height / 2, &unit_rect);

	/*iPoint goal_world = App->map->MapToWorld(this->info.goal_tile.x, this->info.goal_tile.y);

	App->render->Blit(App->scene->debug_tex2,goal_world.x, goal_world.y);*/
}

void j1Unit::LogicUpdate(float dt)
{
	if (this->info.current_group != nullptr)
	{
		if (info.current_group->IsGroupLead(this))
			info.current_group->CheckForMovementRequest(dt);
	}
}

void j1Unit::DrawQuad()
{
	const SDL_Rect entityrect = { Entityinfo.position.x + App->map->data.tile_width / 3,  Entityinfo.position.y + App->map->data.tile_height / 2,  Entityinfo.Size.x,  Entityinfo.Size.y };
	App->render->DrawQuad(entityrect, unitinfo.color.r, unitinfo.color.g, unitinfo.color.b, unitinfo.color.a,false);
}

// --- UnitInfo Constructors and Destructor ---
UnitInfo::UnitInfo(){}

UnitInfo::~UnitInfo(){}

UnitInfo::UnitInfo(const UnitInfo &info): color (info.color){}
