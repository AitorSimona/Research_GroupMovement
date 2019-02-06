#include "j1Collision.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "Brofiler/Brofiler.h"


j1Collision::j1Collision()
{
	name.create("collision");

	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)][static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)][static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)] = false;

	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)][static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)][static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)] = false;

	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)][static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)][static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)] = false;


	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)][static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)][static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)] = false;

	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)][static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)][static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)] = true;
    matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)] = true;


	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)][static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)][static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)] = false;


	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)][static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)][static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)] = true;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)] = true;

	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ORB)][static_cast<int>(COLLIDER_TYPE::COLLIDER_FLOOR)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ORB)][static_cast<int>(COLLIDER_TYPE::COLLIDER_SPIKES)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ORB)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLATFORM)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ORB)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ROOF)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ORB)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_BAT)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ORB)][static_cast<int>(COLLIDER_TYPE::COLLIDER_ENEMY_SLIME)] = false;
	matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_ORB)][static_cast<int>(COLLIDER_TYPE::COLLIDER_PLAYER)] = true;

}

j1Collision::~j1Collision()
{
}

bool j1Collision::PreUpdate()
{
	BROFILER_CATEGORY("Collision_PreUpdate", Profiler::Color::DarkGoldenRod);

	bool ret = true;

	// Remove all colliders scheduled for deletion

	p2List_item <Collider*> *item;
	item = colliders.start;

	while (item != NULL)
	{
		if (item->data->to_delete == true)
			colliders.del(item);

		item = item->next;
	}


	return ret;
}

bool j1Collision::Update(float dt)
{
	BROFILER_CATEGORY("Collision_Update", Profiler::Color::DarkCyan);

	
	return true;
}

bool j1Collision::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Collision_Post_Update", Profiler::Color::DarkGray);

	DebugDraw();

	return true;
}


bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	p2List_item <Collider*> *item;
	item = colliders.start;

	while (item != NULL)
	{
		if (item->data->type == COLLIDER_TYPE::COLLIDER_ORB)
		{
			//testing where does it fail
			int lol=0;
		}
		RELEASE(item->data);
		item = item->next;
	}
	colliders.clear();


	return true;
}

Collider * j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module * callback)
{

	Collider * to_Add = new Collider(rect, type, callback);

	colliders.add(to_Add);

	return to_Add;
}

void j1Collision::DebugDraw()
{

	BROFILER_CATEGORY("Collision_Debug_Draw", Profiler::Color::BurlyWood);

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) //collider draw
		debug = !debug;

	if (debug == false)
		return;

	p2List_item <Collider*> *item;
	item = colliders.start;

	Uint8 alpha = 50;

	while (item!=NULL)
	{

		switch (item->data->type)
		{
		case COLLIDER_TYPE::COLLIDER_NONE: // white
			App->render->DrawQuad(item->data->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_TYPE::COLLIDER_FLOOR: // red
			App->render->DrawQuad(item->data->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_TYPE::COLLIDER_PLAYER: // green
			App->render->DrawQuad(item->data->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_TYPE::COLLIDER_SPIKES: // yellow
			App->render->DrawQuad(item->data->rect, 255, 255, 0, alpha);
			break;
		case COLLIDER_TYPE::COLLIDER_PLATFORM: // magenta
			App->render->DrawQuad(item->data->rect, 255, 0, 255, alpha);
			break;
		case COLLIDER_TYPE::COLLIDER_ROOF: // rose
			App->render->DrawQuad(item->data->rect, 255, 0, 128, alpha);
			break;
		case COLLIDER_TYPE::COLLIDER_ENEMY_SLIME: // brown
			App->render->DrawQuad(item->data->rect, 153, 76, 0, alpha);
			break;
		case COLLIDER_TYPE::COLLIDER_ENEMY_BAT: // dark red
			App->render->DrawQuad(item->data->rect, 153, 0, 76, alpha);
			break;
		case COLLIDER_TYPE::COLLIDER_ORB: // blue
			App->render->DrawQuad(item->data->rect, 0, 191, 255, alpha);
			break;
		}
		item = item->next;
	}

}

void j1Collision::QueryCollisions(Collider & to_check) const
{			
	BROFILER_CATEGORY("Collision_Query", Profiler::Color::CornflowerBlue);

	p2List_item <Collider*> *collider_node = colliders.start;

	while (collider_node)
	{
		//--- Only check area near entity ---

			// Target Collision    ------------------------------   Set Area surrounding Entity
		if (    &to_check != collider_node->data &&
			    collider_node->data->rect.x                               <= to_check.rect.x + App->scene->areaofcollision                      &&
				collider_node->data->rect.x + collider_node->data->rect.w >= to_check.rect.x - App->scene->areaofcollision                    &&
				collider_node->data->rect.y								  <= to_check.rect.y + to_check.rect.h + App->scene->areaofcollision &&
				collider_node->data->rect.y + collider_node->data->rect.h >= to_check.rect.y - App->scene->areaofcollision)
		{

			if (to_check.CheckCollision(collider_node->data->rect) == true)
			{
				if (to_check.callback && matrix[static_cast<int>(to_check.type)][static_cast<int>(collider_node->data->type)])
				{
					to_check.callback->OnCollision(&to_check, collider_node->data);
				}

				if (collider_node->data->callback && matrix[static_cast<int>(collider_node->data->type)][static_cast<int>(to_check.type)])
				{
					collider_node->data->callback->OnCollision(collider_node->data, &to_check);
				}
			}

		}
		collider_node = collider_node->next;
	}

}


bool Collider::CheckCollision(const SDL_Rect & r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);

   //	return SDL_HasIntersection(&rect, &r);

	// between argument "r" and property "rect"
}
