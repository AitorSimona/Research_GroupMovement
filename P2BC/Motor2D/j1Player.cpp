#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1EntityManager.h"
#include "j1Audio.h"


j1Player::j1Player() : j1Entity("player",entity_type::PLAYER)
{
}

j1Player::~j1Player()
{

}

bool j1Player::Start()
{
	LOG("--- Loading player ---");

	// --- Get every data loaded by Entity Manager ---
	playerinfo = manager->GetPlayerData();

	// --- Player's bounding box ---
	entitycollrect = playerinfo.playerrect;
	entitycoll = App->coll->AddCollider(entitycollrect,COLLIDER_TYPE::COLLIDER_PLAYER, (j1Module*) manager);

	// --- Current Player Position ---
	position.x = 0;
	position.y = 0;

	entitycoll->SetPos(position.x, position.y);

	Future_position.x = position.x;
	Future_position.y = position.y;

	// --- Currently playing Animation ---
	CurrentAnimation = playerinfo.idleRight;
	
	// --- Entity Spritesheet ---
	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(playerinfo.Texture.GetString());

	// --- Entity ID for save purposes ---
	entityID = App->entities->entityID;
	
	// --- Entity Velocity ---
	Velocity.x = playerinfo.Velocity.x;
	Velocity.y = playerinfo.Velocity.y;

	// -- Player lifes ----
	lifes = 3;

	//-- active ----
	active = true;


	// -- score ---
	score = 0;
	totalscore = 0;

	return true;
}

void j1Player::UpdateEntityMovement(float dt)
{

	switch (EntityMovement)
	{
		case MOVEMENT::RIGHTWARDS:
			Accumulative_pos_Right += Velocity.x*dt;

			if (Accumulative_pos_Right > 1.1)
			{
				Future_position.x += Accumulative_pos_Right;
				Accumulative_pos_Right -= Accumulative_pos_Right;
			}
			break;
		case MOVEMENT::LEFTWARDS:
			Accumulative_pos_Left += Velocity.x*dt;
			
			if (on_air)
			{
				if (Accumulative_pos_Left > 1.0)
				{
					Future_position.x -= Accumulative_pos_Left;
					Future_position.x -= Accumulative_pos_Left;

					Accumulative_pos_Left -= Accumulative_pos_Left;
				}
			}
			else
			{
				if (Accumulative_pos_Left > 1.5)
				{
					Future_position.x -= Accumulative_pos_Left;
					Accumulative_pos_Left -= Accumulative_pos_Left;
				}
			}
			break;
		case MOVEMENT::UPWARDS:

			Accumulative_pos_Up += Velocity.y*dt;

			if (Accumulative_pos_Up > 0.75)
			{
				Future_position.y -= Accumulative_pos_Up;
				Accumulative_pos_Up -= Accumulative_pos_Up;
			}
			break;
		case MOVEMENT::FREEFALL:

			Accumulative_pos_Down += playerinfo.gravity* dt;

			if(on_air)
			Accumulative_pos_Down += playerinfo.gravity * dt;

			if (Accumulative_pos_Down > 1.0)
			{
				Velocity.y -= Accumulative_pos_Down;
				Future_position.y += Accumulative_pos_Down;
				Accumulative_pos_Down -= Accumulative_pos_Down;
			}
			
			break;
	}

	entitycoll->SetPos(Future_position.x, Future_position.y);

	App->coll->QueryCollisions(*entitycoll);

	MOVEMENT EntityMovement = MOVEMENT::STATIC;
}

void j1Player::God_Movement(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		Future_position.x += Velocity.x*3*dt;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		Future_position.x -= Velocity.x*3*dt;


	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		Future_position.y -= Velocity.x*3.0f*dt;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		Future_position.y += Velocity.x*3.0f*dt;

}

inline void j1Player::Apply_Vertical_Impulse(float dt)
{
	Velocity.y += playerinfo.jump_force;
}

void j1Player::Handle_Ground_Animations()
{
	// --- Handling Ground Animations ---
	
		//--- TO RUN ---

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			if (CurrentAnimation == playerinfo.runRight)
				CurrentAnimation = playerinfo.idleRight;
			else if (CurrentAnimation == playerinfo.runLeft)
				CurrentAnimation = playerinfo.idleLeft;
		}
		else
		{

			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT &&
				CurrentAnimation != playerinfo.jumpingRight      &&
				CurrentAnimation != playerinfo.fallingRight)
			{
				CurrentAnimation = playerinfo.runRight;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT &&
				CurrentAnimation != playerinfo.jumpingLeft       &&
				CurrentAnimation != playerinfo.fallingLeft)
			{
				CurrentAnimation = playerinfo.runLeft;
			}
		}

		//--- TO IDLE ---

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP && CurrentAnimation == playerinfo.runRight || CurrentAnimation == playerinfo.jumpingRight)
			CurrentAnimation = playerinfo.idleRight;

		if (CurrentAnimation == playerinfo.fallingRight)
			CurrentAnimation = playerinfo.idleRight;

		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP && CurrentAnimation == playerinfo.runLeft || CurrentAnimation == playerinfo.jumpingLeft)
 			CurrentAnimation = playerinfo.idleLeft;
		
		if (CurrentAnimation == playerinfo.fallingLeft)
			CurrentAnimation = playerinfo.idleLeft;

    //--------------    ---------------

	//testing life system

	/*	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT)
		{
			playerinfo.deathRight->Reset();
			playerinfo.deathLeft->Reset();
			CurrentAnimation = playerinfo.deathRight;
			lifes -= 1;
			dead = true;
			score -= 100;
			
		}*/
		

		if (lifes == 0)
		{
			App->scene->Activate_MainMenu = true;
			App->scene->Activate_HUD = false;
		}

		if (lifes < 0 && CurrentAnimation->Finished())
		{
			lifes = 0;
			dead = false;
		}
		else if (dead == true && CurrentAnimation->Finished())
		{

			dead = false;
			bool success = App->LoadGame("save_game.xml");
			if (!success)
			{
				App->scene->change_scene(App->scene->StageList.start->data->GetString());
				App->scene->firstStage = true;
				App->scene->secondStage = false;
			}
			else
				App->LoadGame("save_game.xml");

			CurrentAnimation = playerinfo.idleRight;
		}
		
}


void j1Player::Handle_Aerial_Animations()
{
	// --- Handling Aerial Animations ---

	
		//--- TO JUMP ---

		if (Velocity.y > playerinfo.jump_force / 2)
		{
			if (CurrentAnimation == playerinfo.runRight || CurrentAnimation == playerinfo.idleRight)
				CurrentAnimation = playerinfo.jumpingRight;

			else if (CurrentAnimation == playerinfo.runLeft || CurrentAnimation == playerinfo.idleLeft)
				CurrentAnimation = playerinfo.jumpingLeft;
		}

		//--- TO FALL ---

		else if (Velocity.y < playerinfo.jump_force / 2)
		{
			if (CurrentAnimation == playerinfo.jumpingRight || CurrentAnimation == playerinfo.runRight || CurrentAnimation == playerinfo.idleRight)
				CurrentAnimation = playerinfo.fallingRight;
			else if (CurrentAnimation == playerinfo.jumpingLeft || CurrentAnimation == playerinfo.runLeft || CurrentAnimation == playerinfo.idleLeft)
				CurrentAnimation = playerinfo.fallingLeft;
		}


	//------------ --------------
}

bool j1Player::Update(float dt)
{
	// --- LOGIC --------------------

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if (!App->scene->Activate_MainMenu
			&& !App->scene->Activate_Ingamemenu
			&& !App->scene->Activate_Credits
			&& !App->scene->Activate_InGameSettings
			&& !App->scene->Activate_MainMenuSettings)
		{
			god_mode = !god_mode;
		}
	}

	if (god_mode)
	{
		God_Movement(dt);
	}

	else
	{

		// --- RIGHT --
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT &&
			App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
		{
			EntityMovement = MOVEMENT::RIGHTWARDS;
		}

		if (EntityMovement != MOVEMENT::STATIC)
			UpdateEntityMovement(dt);

		// --- LEFT ---
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT &&
			App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
		{
			EntityMovement = MOVEMENT::LEFTWARDS;
		}

		if (EntityMovement != MOVEMENT::STATIC)
			UpdateEntityMovement(dt);

		// --- IMPULSE ---
		if (!on_air && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			App->audio->PlayFx(App->audio->jumpfx);
			Apply_Vertical_Impulse(dt);
			on_air = true;
		}

		else if (on_air && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && double_jump==false)
		{
			App->audio->PlayFx(App->audio->doublejumpfx);
			double_jump = true;
			Velocity.y = 0.0f;
			Apply_Vertical_Impulse(dt);
		}

		// --- UP ---
		if (on_air && Velocity.y > 0.0f)
		{
			EntityMovement = MOVEMENT::UPWARDS;
		}

		if (EntityMovement != MOVEMENT::STATIC)
			UpdateEntityMovement(dt);


		// --- FREE FALL --- 


			for (unsigned short i = 0; i < 4; ++i)
			{
				EntityMovement = MOVEMENT::FREEFALL;
				UpdateEntityMovement(dt);

				if (coll_up)
					break;

			}
	}

	//-------------------------------

	// --- Handling animations ---

	if(Velocity.y == 0.0f)
	Handle_Ground_Animations();
	else
	Handle_Aerial_Animations();

	
	return true;
}

bool j1Player::PostUpdate(float dt)
{
	bool ret = true;

	// --- Parallax movement ---

	previousflow = parallaxflow;
	parallaxflow = -App->render->camera.x/App->win->GetScale() - App->map->offset;

	if (App->scene->firstStage)
	{
		App->map->paralaxRef[0] -= (parallaxflow-previousflow) / 10.0f;
		App->map->paralaxRef[1] -= (parallaxflow-previousflow) / 3.0f;

	}
	else if (App->scene->secondStage)
	{
		App->map->paralaxRef[0] -= (parallaxflow - previousflow) / 10.0f;
		App->map->paralaxRef[1] -= (parallaxflow - previousflow) / 3.0f;
	}

	// ---------------------- //

	// --- Blitting player ---

	
		if (CurrentAnimation == playerinfo.runRight || CurrentAnimation == playerinfo.idleRight || CurrentAnimation == playerinfo.fallingRight || CurrentAnimation == playerinfo.jumpingRight)
			App->render->Blit(spritesheet, Future_position.x - 3, Future_position.y, &CurrentAnimation->GetCurrentFrame(dt));

		else if (CurrentAnimation == playerinfo.runLeft || CurrentAnimation == playerinfo.idleLeft || CurrentAnimation == playerinfo.fallingLeft || CurrentAnimation == playerinfo.jumpingLeft)
			App->render->Blit(spritesheet, Future_position.x - 6, Future_position.y, &CurrentAnimation->GetCurrentFrame(dt));

		else
			App->render->Blit(spritesheet, Future_position.x - 3, Future_position.y, &CurrentAnimation->GetCurrentFrame(dt));
	
	
	// ---------------------- //

	return ret;
}

void j1Player::OnCollision(Collider * entitycollider, Collider * to_check)
{
	if (!god_mode)
	{
		switch (EntityMovement)
		{
		case MOVEMENT::RIGHTWARDS:
			Right_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::LEFTWARDS:
			Left_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::UPWARDS:
			Up_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::FREEFALL:
			Down_Collision(entitycollider, to_check);
			break;
		}

		Future_position.x = entitycollider->rect.x;
		Future_position.y = entitycollider->rect.y;
	}
}

void j1Player::Right_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.x -= Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
		case COLLIDER_TYPE::COLLIDER_PLATFORM:
			entitycollider->rect.x -= Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
		case COLLIDER_TYPE::COLLIDER_ROOF:
			entitycollider->rect.x -= Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
	}
}

void j1Player::Left_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.x += Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
		case COLLIDER_TYPE::COLLIDER_PLATFORM:
			entitycollider->rect.x += Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
		case COLLIDER_TYPE::COLLIDER_ROOF:
			entitycollider->rect.x += Intersection.w;
			App->render->camera.x = camera_pos_backup.x;
			break;
	}
}

void j1Player::Up_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	coll_up = true;

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.y += Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_PLATFORM:
			entitycollider->rect.y += Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_ROOF:
			entitycollider->rect.y += Intersection.h;
			break;
	}

	Velocity.y = 0.0f;
}

void j1Player::Down_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.y -= Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_PLATFORM:
			entitycollider->rect.y -= Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_ROOF:
			entitycollider->rect.y -= Intersection.h;
			break;
		case COLLIDER_TYPE::COLLIDER_SPIKES:
			entitycollider->rect.y -= Intersection.h;
		
			if (!dead)
			{
				App->audio->PlayFx(App->audio->deathfx);
				LOG("actual lifes. %i", lifes);
				playerinfo.deathRight->Reset();
				CurrentAnimation = playerinfo.deathRight;
				Velocity.y += playerinfo.jump_force;
				CurrentAnimation = playerinfo.deathRight;
				lifes -= 1;
				LOG("now lifes. %i", App->scene->player->lifes);
				dead = true;
				score -= 100;
			}
			break;
	}

	double_jump = false;
	coll_up = false;
	Velocity.y = 0.0f;
	on_air = false;
}

bool j1Player::Load(pugi::xml_node &config)
{
	Future_position.x= config.child("Player").child("Playerx").attribute("value").as_float();
	Future_position.y = config.child("Player").child("Playery").attribute("value").as_float();
	orbs_number = config.child("Player").child("orbs_number").attribute("value").as_int();
	score = config.child("Player").child("score").attribute("value").as_int();
	TimeAuxload =config.child("Player").child("time").attribute("value").as_uint();
	TimePausedSave = config.child("Player").child("timePaused").attribute("value").as_uint();
	//lifes = config.child("Player").child("life").attribute("value").as_int();

	//App->scene->sceneTimer.Loadstart(TimeAuxload);
	return true;
}

bool j1Player::Save(pugi::xml_node &config) const
{ 
	config.append_child("Player").append_child("Playerx").append_attribute("value")= Future_position.x;
	config.child("Player").append_child("Playery").append_attribute("value")= Future_position.y;
	config.child("Player").append_child("orbs_number").append_attribute("value") = orbs_number;
	config.child("Player").append_child("score").append_attribute("value") = score;
	//config.child("Player").append_child("life").append_attribute("value") = lifes;
	config.child("Player").append_child("time").append_attribute("value") = App->scene->sceneTimer.Read();
	config.child("Player").append_child("timePaused").append_attribute("value") = App->scene->sceneTimer.getPausd();

	return true;
}

bool j1Player::CleanUp()
{
	App->tex->UnLoad(spritesheet);

	if(entitycoll != nullptr)
	entitycoll = nullptr;

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

	camera_pos_backup.x = App->render->camera.x;
	camera_pos_backup.y = App->render->camera.y;

	EntityMovement = MOVEMENT::STATIC;

	Update(dt);
}

