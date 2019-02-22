#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1EntityManager.h"
#include "j1Audio.h"
#include <time.h>

j1Player::j1Player() : j1Entity("player", entity_type::PLAYER)
{
}

j1Player::~j1Player()
{

}

bool j1Player::Start()
{
	LOG("--- Loading player ---");

	// --- Current Player Position ---
	position.x = rand() % 300;
	position.y = rand() % 300;

	Future_position.x = position.x;
	Future_position.y = position.y;

	entitycollrect.x = 56;
	entitycollrect.y = 12;
	entitycollrect.w = 16;
	entitycollrect.h = 32;

	const char * stri = "textures/spritesheet.png";
	// --- Entity Spritesheet ---
	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(stri);

	// --- Entity Velocity ---
	Velocity.x = 5;
	Velocity.y = 0;

	//-- active ----
	active = true;


	return true;
}

void j1Player::UpdateEntityMovement(float dt)
{

	
}


bool j1Player::Update(float dt)
{
	// --- LOGIC --------------------

	
	return true;
}

bool j1Player::PostUpdate(float dt)
{
	bool ret = true;

	// --- Blitting player ---
	App->render->Blit(spritesheet, position.x, position.y, &entitycollrect);
	// ---------------------- //

	return ret;
}


bool j1Player::CleanUp()
{
	App->tex->UnLoad(spritesheet);

	return true;
}

void j1Player::FixedUpdate(float dt)
{
	// --- Update we do every frame ---

	PostUpdate(dt);

}

void j1Player::LogicUpdate(float dt)
{
	// --- Update we may not do every frame ---

	Update(dt);
}

