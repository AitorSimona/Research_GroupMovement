#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "j1PathFinding.h"
#include "j1EntityManager.h"
#include "Brofiler\Brofiler.h"
#include "j1Button.h"
#include "j1Fonts.h"



j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	for (pugi::xml_node stage = config.child("map_name"); stage; stage = stage.next_sibling("map_name"))
	{
		p2SString* StageName = new p2SString(stage.attribute("name").as_string());
		StageList.add(StageName);
			App->map->numberStages++;
	}


	if (StageList.start->data->GetString() == NULL)
	{
		ret = false;
		LOG("stagelist is null");
	}
	camera1.x = config.child("startcamera1").attribute("x").as_int();
	camera1.y = config.child("startcamera1").attribute("y").as_int();
	camera2.x = config.child("startcamera2").attribute("x").as_int();
	camera2.y = config.child("startcamera2").attribute("y").as_int();

	areaofcollision = config.child("areaofcollision").attribute("value").as_int();

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = true;


	// --- Creating entity  ---
	player = (j1Player*)App->entities->CreateEntity("player", entity_type::PLAYER);

	//Loading both maps

	p2List_item<p2SString*>* stageListItem;
	stageListItem = StageList.start;

	ret = App->map->Load(stageListItem->data->GetString(),App->map->data);
		
		if (ret == false)
		{
			LOG("issue loading First Stage");
			ret = false;
		}
		
	stageListItem = StageList.start->next;;

	ret = App->map->Load(stageListItem->data->GetString(), App->map->data2);

	if (ret == false)
	{
		LOG("issue loading Second Stage");
		ret = false;
	}


	//loading music & positions depending on the order

	FirstStage = StageList.start->data->GetString();

	if (FirstStage =="stage1_TiledV017.tmx" )
	{
		App->render->camera.x = camera1.x;
		App->render->camera.y = camera1.y;

		player->Future_position.x = App->map->data.initpos.x;
		player->Future_position.y = App->map->data.initpos.y;

		// --- Pathfinding walkability map 1 ---

		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data,App->map->data))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
	}
	else
	{
		App->render->camera.x = camera2.x;
		App->render->camera.y = camera2.y;

		player->Future_position.x = App->map->data2.initpos.x;
		player->Future_position.y = App->map->data2.initpos.y;

		// --- Pathfinding walkability map 2 ---

		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data, App->map->data2))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
	}

	// --- Initial position for enemies ---

	App->map->ColliderDrawer(App->map->data);

	// --- Loading UI config file ---
	pugi::xml_document	config_file;
	pugi::xml_node		config;

	config = App->LoadConfig(config_file, "UI_Elems.xml");
	App->gui->DeployUI(config);
	MenuMusic.create("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->next->next->data->GetString());
	stage1Music.create("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->data->GetString());
	stage2Music.create("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->next->data->GetString());

	// ---- Timer ----
	sceneTimer.Start();

	// ---- Fade -----
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	App->win->GetWindowSize(window_w, window_h);
	scale = App->win->GetScale();
	screen = { 0, 0, (int)window_w * (int)scale, (int)window_h * (int)scale };
	//Fade(245, 245, 245, 3.0);

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	BROFILER_CATEGORY("Scene_Pre__Update", Profiler::Color::MediumSeaGreen);
	
	// if player loses all his lifes start from the begining of the first map.
	// should be the main menu
	if (player->lifes <= 0)
	{
		change_scene(StageList.start->data->GetString());
		firstStage = true;
		secondStage = false;
		player->lifes = 3;
		no_continue = true;
	}
	
	// debug pathfing ------------------
	//static iPoint origin;
	//static bool origin_selected = false;

	//int x, y;
	//App->input->GetMousePosition(x, y);
	//iPoint p = App->render->ScreenToWorld(x, y);
	//if(firstStage)
	//p = App->map->WorldToMap(p.x, p.y, App->map->data);
	//else
	//p = App->map->WorldToMap(p.x, p.y, App->map->data2);

	//if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	//{
	//	if (origin_selected == true)
	//	{
	//		App->pathfinding->CreatePath(origin, p);
	//		origin_selected = false;
	//	}
	//	else
	//	{
	//		origin = p;
	//		origin_selected = true;
	//	}
	//}

			//Checkpoint
			// Player pos    ------------------------------   area around checkpoint
	if (//checkoint 1
		player->Future_position.x <= App->map->data.checkpoint1.x + 5        &&
		player->Future_position.x >= App->map->data.checkpoint1.x - 5        &&
		player->Future_position.y <= App->map->data.checkpoint1.y + App->map->data.tile_height*2			&&
		player->Future_position.y + App->map->data.tile_height >= App->map->data.checkpoint1.y - App->map->data.tile_height*2 
		||//checkoint 2
		player->Future_position.x <= App->map->data.checkpoint2.x + 5 &&
		player->Future_position.x >= App->map->data.checkpoint2.x - 5 &&
		player->Future_position.y <= App->map->data.checkpoint2.y + App->map->data.tile_height * 2 &&
		player->Future_position.y + App->map->data.tile_height >= App->map->data.checkpoint2.y - App->map->data.tile_height * 2
		|| //checkoint 3
		player->Future_position.x <= App->map->data.checkpoint3.x + 5 &&
		player->Future_position.x >= App->map->data.checkpoint3.x - 5 &&
		player->Future_position.y <= App->map->data.checkpoint3.y + App->map->data.tile_height * 2 &&
		player->Future_position.y + App->map->data.tile_height >= App->map->data.checkpoint3.y - App->map->data.tile_height * 2
		||
		//checkoint 1 second map
		player->Future_position.x <= App->map->data2.checkpoint1.x + 5 &&
		player->Future_position.x >= App->map->data2.checkpoint1.x - 5 &&
		player->Future_position.y <= App->map->data2.checkpoint1.y + App->map->data2.tile_height * 2 &&
		player->Future_position.y + App->map->data2.tile_height >= App->map->data2.checkpoint1.y - App->map->data2.tile_height * 2
		||//checkoint 2 second map
		player->Future_position.x <= App->map->data2.checkpoint2.x + 5 &&
		player->Future_position.x >= App->map->data2.checkpoint2.x - 5 &&
		player->Future_position.y <= App->map->data2.checkpoint2.y + App->map->data2.tile_height * 2 &&
		player->Future_position.y + App->map->data2.tile_height >= App->map->data2.checkpoint2.y - App->map->data2.tile_height * 2
		|| //checkoint 3 second map
		player->Future_position.x <= App->map->data2.checkpoint3.x + 5 &&
		player->Future_position.x >= App->map->data2.checkpoint3.x - 5 &&
		player->Future_position.y <= App->map->data2.checkpoint3.y + App->map->data2.tile_height * 2 &&
		player->Future_position.y + App->map->data2.tile_height >= App->map->data2.checkpoint3.y - App->map->data2.tile_height * 2)

	{
		App->SaveGame("save_game.xml");
	}


	//win condition
	if (firstStage && (player->Future_position.x >= App->map->data.finalpos.x) && (player->Future_position.y <= App->map->data.finalpos.y))
	{
		player->totalscore = player->score;
		change_scene(StageList.start->next->data->GetString());
		firstStage = false;
		secondStage = true;
	}

	else if (secondStage && (player->Future_position.x >= App->map->data2.finalpos.x) && (player->Future_position.y <= App->map->data2.finalpos.y))
	{
		player->totalscore += player->score;
		change_scene(StageList.start->data->GetString());
		firstStage = true;
		secondStage = false;
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene_Update", Profiler::Color::MediumSpringGreen);

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && firstStage == false) //can only press during second stage. goes to first stage
	{	
		if (!Activate_MainMenu
			&& !Activate_Ingamemenu
			&& !Activate_Credits
			&& !Activate_InGameSettings
			&& !Activate_MainMenuSettings)
		{
			player->lifes = 3;

			change_scene(StageList.start->data->GetString());
			firstStage = true;
			secondStage = false;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && secondStage== false) //can only press during first stage. goes to second stage
	{	
		if (!Activate_MainMenu
			&& !Activate_Ingamemenu
			&& !Activate_Credits
			&& !Activate_InGameSettings
			&& !Activate_MainMenuSettings)
		{
			player->lifes = 3;

			change_scene(StageList.start->next->data->GetString());
			firstStage = false;
			secondStage = true;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) //reload stage1
	{
		if (!Activate_MainMenu
			&& !Activate_Ingamemenu
			&& !Activate_Credits
			&& !Activate_InGameSettings
			&& !Activate_MainMenuSettings)
		{
			player->lifes = 3;

			change_scene(StageList.start->data->GetString());
			firstStage = true;
			secondStage = false;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) // beginning of current level
	{
		if (!Activate_MainMenu
			&& !Activate_Ingamemenu
			&& !Activate_Credits
			&& !Activate_InGameSettings
			&& !Activate_MainMenuSettings)
		{
			player->lifes = 3;

			if (firstStage)
			{
				change_scene(StageList.start->data->GetString());
				firstStage = true;
				secondStage = false;
			}
			else if (secondStage)
			{
				change_scene(StageList.start->next->data->GetString());
				firstStage = false;
				secondStage = true;
			}
		}
	}

	/*if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN) 
	{
		App->audio->ChangeVolume_music(10);
		App->audio->ChangeVolume_fx(10);
		LOG("volume up");
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN)
	{
		App->audio->ChangeVolume_music(-10);
		App->audio->ChangeVolume_fx(-10);
		LOG("volume down");
	}*/

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{	
		if (!Activate_MainMenu
			&& !Activate_Ingamemenu
			&& !Activate_Credits
			&& !Activate_InGameSettings
			&& !Activate_MainMenuSettings)
		{
			bool ret = App->LoadGame("save_game.xml");
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		if (!Activate_MainMenu
			&& !Activate_Ingamemenu
			&& !Activate_Credits
			&& !Activate_InGameSettings
			&& !Activate_MainMenuSettings)
		{
			App->SaveGame("save_game.xml");
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
		App->cap_on = !App->cap_on;

	//if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	//	App->render->camera.y += 2;

	//if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	//	App->render->camera.y -= 2;

	//if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	//	App->render->camera.x -= 2;

	//if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	//	App->render->camera.x += 2;

	int x, y;
	App->input->GetMousePosition(x, y);
	x = x / App->win->GetScale();
	y = y / App->win->GetScale();


	if (firstStage == true)
	{
		App->map->Draw(App->map->data);


		iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y, App->map->data);
		p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d Time:%d",
			App->map->data.width, App->map->data.height,
			App->map->data.tile_width, App->map->data.tile_height,
			App->map->data.tilesets.count(),
			map_coordinates.x, map_coordinates.y,
			sceneTimer.Read()
		);
		//Debug purpose
		//App->win->SetTitle(title.GetString());
	}
	else
	{
		App->map->Draw(App->map->data2);


		iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y, App->map->data2);
		p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
			App->map->data.width, App->map->data.height,
			App->map->data.tile_width, App->map->data.tile_height,
			App->map->data.tilesets.count(),
			map_coordinates.x, map_coordinates.y);
		//Debug purpose
		//App->win->SetTitle(title.GetString());
	}

	// --- Debug Pathfinding
	if (App->coll->debug)
	{
		iPoint p = App->render->ScreenToWorld(x, y);
		if (firstStage)
		{
			p = App->map->WorldToMap(p.x, p.y, App->map->data);
			p = App->map->MapToWorld(p.x, p.y, App->map->data);
			App->render->Blit(App->map->data.tilesets.start->next->next->data->texture, p.x, p.y, &debug_Tex_rect);
		}
		else
		{
			p = App->map->WorldToMap(p.x, p.y, App->map->data2);
			p = App->map->MapToWorld(p.x, p.y, App->map->data2);
			App->render->Blit(App->map->data2.tilesets.start->next->next->data->texture, p.x, p.y, &debug_Tex_rect);
		}

		const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

		for (uint i = 0; i < path->Count(); ++i)
		{
			if (firstStage)
			{
				iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y, App->map->data);
				App->render->Blit(App->map->data.tilesets.start->next->next->data->texture, pos.x, pos.y, &debug_Tex_rect);
			}
			else
			{
				iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y, App->map->data2);
				App->render->Blit(App->map->data2.tilesets.start->next->next->data->texture, pos.x, pos.y, &debug_Tex_rect);
			}


		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Scene_Post_Update", Profiler::Color::MediumTurquoise);

	bool ret = true;

	// -- Time paused --
	if (App->on_GamePause)
	{
		if (!taketime)
		{
			taketime = true;
			timeWhenPaused = sceneTimer.ReadSec();
			timeBeingPaused.Start();
		}

	}
	// --- Controlling Music ---

	if (!playing_menu && dt == 0.0f)
	{
		playing_menu = true;
		playing_stage1 = false;
		playing_stage1 = false;
		App->audio->PlayMusic(MenuMusic.GetString());
	}
	else if (dt != 0.0f)
	{
		playing_menu = false;

		if (!playing_stage1 && firstStage)
		{
			playing_stage1 = true;
			playing_stage2 = false;
			App->audio->PlayMusic(stage1Music.GetString());
		}
		else if (!playing_stage2 && secondStage)
		{
			playing_stage1 = false;
			playing_stage2 = true;
			App->audio->PlayMusic(stage2Music.GetString());
		}
	}

	// -------------

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && !Activate_MainMenu)
		Activate_Ingamemenu = true;

	//if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN && !Activate_Ingamemenu)
	//	Activate_MainMenu = !Activate_MainMenu;

	// --- Check if there is any Save Game ---

	pugi::xml_parse_result SaveGame;
	pugi::xml_document SaveGamedoc;

	SaveGame = SaveGamedoc.load_file("save_game.xml");

	if (SaveGame != NULL)
		Existant_SaveGame = true;
	else
		Existant_SaveGame = false;

	// --- Controlling UI BEHAVIOUR ---

	// --- WEBSITE LABEL ---
	if (App->gui->focus == App->gui->UIelements.At(33)->data)
	{
		App->gui->focus = nullptr;
		ShellExecuteA(NULL, "open", "https://atlasworks.github.io/orBit/", NULL, NULL, SW_SHOWNORMAL);
	}

	// --- Quit Button ---
	if (App->gui->focus == App->gui->UIelements.At(18)->data)
	{
		App->gui->focus = nullptr;
		ret = false;
	}

	// --- In Game Continue ---
	if (App->gui->focus == App->gui->UIelements.At(20)->data)
	{
		App->gui->focus = nullptr;
		Activate_Ingamemenu = false;
	}

	// --- Main Menu Continue ---
	if (App->gui->focus == App->gui->UIelements.At(40)->data)
	{
		App->gui->focus = nullptr;
		Activate_MainMenu = false;
		Activate_HUD = true;
		App->LoadGame("save_game.xml");
	}

	// --- Main Menu START ---
	if (App->gui->focus == App->gui->UIelements.At(4)->data)
	{
		App->gui->focus = nullptr;
		Activate_MainMenu = false;
		Activate_HUD = true;
		change_scene(StageList.start->data->GetString());
		firstStage = true;
		secondStage = false;
		no_continue = false;
	}

	// --- In Game SETTINGS TRIGGER ---
	if (App->gui->focus == App->gui->UIelements.At(7)->data)
	{
		App->gui->focus = nullptr;
		Activate_Ingamemenu = false;
		Activate_InGameSettings = true;
	}

	// --- Main Menu SETTINGS TRIGGER ---
	if (App->gui->focus == App->gui->UIelements.At(6)->data)
	{
		App->gui->focus = nullptr;
		Activate_MainMenu = false;
		Activate_MainMenuSettings = true;
	}

	// --- Main Menu CREDITS TRIGGER ---
	if (App->gui->focus == App->gui->UIelements.At(5)->data)
	{
		App->gui->focus = nullptr;
		Activate_MainMenu = false;
		Activate_Credits = true;
	}

	// --- In Game BACK TO IN GAME MENU ---
	if (App->gui->focus == App->gui->UIelements.At(13)->data)
	{
		App->gui->focus = nullptr;
		Activate_Ingamemenu = true;
		Activate_InGameSettings = false;
	}
	// --- In Game BACK TO MAIN MENU TRIGGER ---
	if (App->gui->focus == App->gui->UIelements.At(2)->data)
	{
		App->gui->focus = nullptr;
		Activate_MainMenu = true;
		Activate_HUD = false;
		Activate_Ingamemenu = false;
	}

	// --- Main Menu BACK TO MAIN MENU TRIGGER (CREDITS) ---
	if (App->gui->focus == App->gui->UIelements.At(17)->data)
	{
		App->gui->focus = nullptr;
		Activate_MainMenu = true;
		Activate_HUD = false;
		Activate_Credits = false;
	}

	// --- Main Menu BACK TO MAIN MENU TRIGGER (SETTINGS) ---
	if (App->gui->focus == App->gui->UIelements.At(32)->data)
	{
		App->gui->focus = nullptr;
		Activate_MainMenu = true;
		Activate_HUD = false;
		Activate_MainMenuSettings = false;
	}

	// --- Controlling In-Game Menu ---

	if (Activate_Ingamemenu)
	{
		*App->gui->UIelements.At(2)->data->GetActive() = true;
		*App->gui->UIelements.At(7)->data->GetActive() = true;
		*App->gui->UIelements.At(20)->data->GetActive() = true;

		// --- Labels ---
		*App->gui->UIelements.At(23)->data->GetActive() = true;
		*App->gui->UIelements.At(24)->data->GetActive() = true;
		*App->gui->UIelements.At(25)->data->GetActive() = true;
	}
	else
	{
		*App->gui->UIelements.At(2)->data->GetActive() = false;
		*App->gui->UIelements.At(7)->data->GetActive() = false;
		*App->gui->UIelements.At(20)->data->GetActive() = false;

		// --- Labels ---
		*App->gui->UIelements.At(23)->data->GetActive() = false;
		*App->gui->UIelements.At(24)->data->GetActive() = false;
		*App->gui->UIelements.At(25)->data->GetActive() = false;
	}

	// --- Controlling Main Menu ---

	if (Activate_MainMenu)
	{
		*App->gui->UIelements.At(0)->data->GetActive() = true;

		if (Existant_SaveGame && !no_continue)
		{
			*App->gui->UIelements.At(40)->data->GetActive() = true;
			*App->gui->UIelements.At(41)->data->GetActive() = true;
		}
		else
		{
			*App->gui->UIelements.At(40)->data->GetActive() = false;
			*App->gui->UIelements.At(41)->data->GetActive() = false;
		}

		*App->gui->UIelements.At(3)->data->GetActive() = true;
		*App->gui->UIelements.At(4)->data->GetActive() = true;
		*App->gui->UIelements.At(5)->data->GetActive() = true;
		*App->gui->UIelements.At(6)->data->GetActive() = true;
		*App->gui->UIelements.At(18)->data->GetActive() = true;

		// --- Labels ---
		*App->gui->UIelements.At(19)->data->GetActive() = true;
		*App->gui->UIelements.At(26)->data->GetActive() = true;
		*App->gui->UIelements.At(27)->data->GetActive() = true;

	}
	else
	{
		*App->gui->UIelements.At(0)->data->GetActive() = false;
														
		*App->gui->UIelements.At(40)->data->GetActive() = false;
														
		*App->gui->UIelements.At(3)->data->GetActive() = false;
		*App->gui->UIelements.At(4)->data->GetActive() = false;
		*App->gui->UIelements.At(5)->data->GetActive() = false;
		*App->gui->UIelements.At(6)->data->GetActive() = false;
		*App->gui->UIelements.At(18)->data->GetActive() = false;

		// --- Labels ---
		*App->gui->UIelements.At(19)->data->GetActive() = false;
		*App->gui->UIelements.At(41)->data->GetActive() = false;
		*App->gui->UIelements.At(26)->data->GetActive() = false;
		*App->gui->UIelements.At(27)->data->GetActive() = false;
	}

	// --- SETTINGS IN GAME PANEL

	if (Activate_InGameSettings)
	{
		*App->gui->UIelements.At(9)->data->GetActive() = true;
		*App->gui->UIelements.At(10)->data->GetActive() = true;
		*App->gui->UIelements.At(11)->data->GetActive() = true;
		*App->gui->UIelements.At(12)->data->GetActive() = true;
		*App->gui->UIelements.At(13)->data->GetActive() = true;
		*App->gui->UIelements.At(37)->data->GetActive() = true;
		*App->gui->UIelements.At(38)->data->GetActive() = true;
		*App->gui->UIelements.At(39)->data->GetActive() = true;
	}
	else
	{
		*App->gui->UIelements.At(9)->data->GetActive() = false;
		*App->gui->UIelements.At(10)->data->GetActive() =false;
		*App->gui->UIelements.At(11)->data->GetActive() =false;
		*App->gui->UIelements.At(12)->data->GetActive() =false;
		*App->gui->UIelements.At(13)->data->GetActive() =false;
		*App->gui->UIelements.At(37)->data->GetActive() = false;
		*App->gui->UIelements.At(38)->data->GetActive() = false;
		*App->gui->UIelements.At(39)->data->GetActive() = false;
	}

	// --- SETTINGS MAIN MENU PANEL ---

	if (Activate_MainMenuSettings)
	{
		*App->gui->UIelements.At(28)->data->GetActive() = true;
		*App->gui->UIelements.At(29)->data->GetActive() = true;
		*App->gui->UIelements.At(30)->data->GetActive() = true;
		*App->gui->UIelements.At(31)->data->GetActive() = true;
		*App->gui->UIelements.At(32)->data->GetActive() = true;
		*App->gui->UIelements.At(34)->data->GetActive() = true;
		*App->gui->UIelements.At(35)->data->GetActive() = true;
		*App->gui->UIelements.At(36)->data->GetActive() = true;
		
	}
	else
	{
		*App->gui->UIelements.At(28)->data->GetActive() = false;
		*App->gui->UIelements.At(29)->data->GetActive() = false;
		*App->gui->UIelements.At(30)->data->GetActive() = false;
		*App->gui->UIelements.At(31)->data->GetActive() = false;
		*App->gui->UIelements.At(32)->data->GetActive() = false;
		*App->gui->UIelements.At(34)->data->GetActive() = false;
		*App->gui->UIelements.At(35)->data->GetActive() = false;
		*App->gui->UIelements.At(36)->data->GetActive() = false;
	}

	// --- CREDITS MAIN MENU PANEL ---

	if (Activate_Credits)
	{
		*App->gui->UIelements.At(8)->data->GetActive() = true;
		*App->gui->UIelements.At(14)->data->GetActive() = true;
		*App->gui->UIelements.At(15)->data->GetActive() = true;
		*App->gui->UIelements.At(16)->data->GetActive() = true;
		*App->gui->UIelements.At(17)->data->GetActive() = true;
		*App->gui->UIelements.At(33)->data->GetActive() = true;
	}
	else
	{
		*App->gui->UIelements.At(8)->data->GetActive() = false;
		*App->gui->UIelements.At(14)->data->GetActive() =false;
		*App->gui->UIelements.At(15)->data->GetActive() =false;
		*App->gui->UIelements.At(16)->data->GetActive() =false;
		*App->gui->UIelements.At(17)->data->GetActive() =false;
		*App->gui->UIelements.At(33)->data->GetActive() = false;
	}

	if (Activate_HUD)
	{
		// --- ORBS ---

		if (player->orbs_number >= 1)
		*App->gui->UIelements.At(1)->data->GetActive() = true;
		else
		*App->gui->UIelements.At(1)->data->GetActive() = false;

		if (player->orbs_number >= 2)
		*App->gui->UIelements.At(21)->data->GetActive() = true;
		else
		*App->gui->UIelements.At(21)->data->GetActive() = false;

		if(player->orbs_number >= 3)
		*App->gui->UIelements.At(22)->data->GetActive() = true;
		else
		*App->gui->UIelements.At(22)->data->GetActive() = false;

		// --- SCORE ---
		*App->gui->UIelements.At(45)->data->GetActive() = true;
		*App->gui->UIelements.At(46)->data->GetActive() = true;

		score_sum = player->score + player->totalscore;
		auxstr = std::to_string(score_sum);
		scorestring=auxstr.data();
		App->gui->UIelements.At(46)->data->ShapeLabel(scorestring);

		// --- LIFES ---

		if(player->lifes >= 1)
		*App->gui->UIelements.At(42)->data->GetActive() = true;
		else
		*App->gui->UIelements.At(42)->data->GetActive() = false;

		if (player->lifes >= 2)
		*App->gui->UIelements.At(43)->data->GetActive() = true;
		else
		*App->gui->UIelements.At(43)->data->GetActive() = false;

		if (player->lifes >= 3)
		*App->gui->UIelements.At(44)->data->GetActive() = true;
		else
		*App->gui->UIelements.At(44)->data->GetActive() = false;

		// --- Timer ---

		*App->gui->UIelements.At(47)->data->GetActive() = true;
		*App->gui->UIelements.At(48)->data->GetActive() = true;

		if (App->on_GamePause)
		{
			auxstr = std::to_string(timeWhenPaused);
			scorestring = auxstr.data();
			App->gui->UIelements.At(48)->data->ShapeLabel(scorestring);
		}
		else
		{
			auxstr = std::to_string(sceneTimer.ReadSec());
			scorestring = auxstr.data();
			App->gui->UIelements.At(48)->data->ShapeLabel(scorestring);
		}
	}
	else
	{
		*App->gui->UIelements.At(1)->data->GetActive() = false;
		*App->gui->UIelements.At(21)->data->GetActive() = false;
		*App->gui->UIelements.At(22)->data->GetActive() = false;
		*App->gui->UIelements.At(42)->data->GetActive() = false;
		*App->gui->UIelements.At(43)->data->GetActive() = false;
		*App->gui->UIelements.At(44)->data->GetActive() = false;

		*App->gui->UIelements.At(45)->data->GetActive() = false;
		*App->gui->UIelements.At(46)->data->GetActive() = false;
		*App->gui->UIelements.At(47)->data->GetActive() = false;
		*App->gui->UIelements.At(48)->data->GetActive() = false;
	}

	// --- Controlling camera Bounds---

	// --- Camera In X ---

	// --- Keeping camera on axis X bounds ---
	

	if (player->Future_position.x*App->win->GetScale() + player->entitycoll->rect.w >= -App->render->camera.x + App->render->camera.w - App->render->camera.w / 3)
	{
		App->render->camera.x = -(player->Future_position.x* App->win->GetScale() + player->entitycoll->rect.w - App->render->camera.w + App->render->camera.w / 3);
	}

	if (player->Future_position.x*App->win->GetScale() <= -App->render->camera.x + App->render->camera.w / 3)
	{
		App->render->camera.x = -(player->Future_position.x* App->win->GetScale() - App->render->camera.w / 3);
	}

	if (-App->render->camera.x <= 2.0f)
	{
		App->render->camera.x = -2.0f;

	}

	if (-App->render->camera.x + App->render->camera.w >= App->map->data.width*App->map->data.tile_width*App->win->GetScale())
	{
		App->render->camera.x = -App->map->data.width*App->map->data.tile_width*App->win->GetScale() + App->render->camera.w;
	}

	// --- Camera In Y ---

	// --- Camera down ---

	if (player->Future_position.y*App->win->GetScale() + player->entitycoll->rect.h >= -App->render->camera.y + App->render->camera.h - App->render->camera.h / 6)
	{
		App->render->camera.y = -(player->Future_position.y* App->win->GetScale() + player->entitycoll->rect.h - App->render->camera.h +App->render->camera.h / 6);
	}

	//// --- Camera up ---

	if (player->Future_position.y*App->win->GetScale() <= -App->render->camera.y + App->render->camera.h / 6)
	{
		App->render->camera.y = -(player->Future_position.y* App->win->GetScale() - App->render->camera.h / 6);
	}

	// --- Keeping camera on axis Y bounds ---

	if (-App->render->camera.y + App->render->camera.h > App->map->data.height*App->map->data.tile_height*App->win->GetScale())
	{
		App->render->camera.y = (-App->map->data.height*App->map->data.tile_height*App->win->GetScale() + App->render->camera.h);
	}

	if (App->render->camera.y > 0.0)
	{
		App->render->camera.y = 0.0f;
	}

	// -- Update for fade ---

	if (fadeState)
	{

		if (current_step == fade_step::none)
			return true;

		Uint32 currentTick = SDL_GetTicks() - start_time;
		float normalized = MIN(1.0f, (float)currentTick / (float)total_time);

		switch (current_step)
		{
		case fade_step::fade_to:
		{
			if (currentTick >= total_time)
			{
				total_time += total_time;
				start_time = SDL_GetTicks();
				current_step = fade_step::fade_from;
			}
		} break;

		case fade_step::fade_from:
		{
			normalized = 1.0f - normalized;

			if (currentTick >= total_time)
			{
				fadeState = false;
				player->playerinfo.dontShow = fadeState;
				current_step = fade_step::none;
			}

		} break;
		}

		// Finally render the black square with alpha on the screen
		SDL_SetRenderDrawColor(App->render->renderer, redAmount, greenAmount, blueAmount, (Uint8)(normalized * 255.0f));
		SDL_RenderFillRect(App->render->renderer, &screen);

		return true;

	}

	//-------------------

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	p2List_item<p2SString*>* item;
	item = StageList.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	StageList.clear();

	return true;
}


bool j1Scene::change_scene(const char* map_name) {
	
	bool ret = true;

	timeAccumulated = 0;
	timeWhenPaused = 0;
	sceneTimer.ResetX();
	sceneTimer.ResetPaused();
	sceneTimer.ResetTotalPasedTime();
	sceneTimer.Start();


	Fade(245, 245, 245, 2.0);

	App->map->paralaxRef[0] = App->map->offset;
	App->map->paralaxRef[1] = App->map->offset;

	player->parallaxflow = 0;
	player->previousflow = 0;
	player->orbs_number = 0; //player must save and load this
	App->coll->CleanUp();

	

	if (FirstStage == map_name)
	{	
		App->render->camera.x = camera1.x;
		App->render->camera.y = camera1.y;

		

		App->map->ColliderDrawer(App->map->data);
		//p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->data->GetString());
		//App->audio->PlayMusic(stageMusic.GetString());
		


		// --- Pathfinding walkability map 1 ---

		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data, App->map->data))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
	}
	else
	{
		App->render->camera.x = camera2.x;
		App->render->camera.y = camera2.y;
		
		App->map->ColliderDrawer(App->map->data2);
		//p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->next->data->GetString());
		//App->audio->PlayMusic(stageMusic.GetString());
		

		// --- Pathfinding walkability map 2 ---

		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data, App->map->data2))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
	}

	player->CurrentAnimation = player->playerinfo.idleRight;

	setStandarEntityPosition(map_name);


	return ret;
}


bool j1Scene::Save(pugi::xml_node &config) const
{
	bool ret = true;

	config.append_child("firstStage").append_attribute("value") = firstStage;
	config.append_child("secondStage").append_attribute("value") = secondStage;


	return ret;
}

bool j1Scene::Load(pugi::xml_node &config)
{

	bool ret = true;
	int x = player->Future_position.x;
	int y = player->Future_position.y;

	DestinationStage1 = config.child("firstStage").attribute("value").as_bool();
	DestinationStage2 = config.child("secondStage").attribute("value").as_bool();

	if (firstStage)
	{

		//stage 2
		if (DestinationStage2)
		{
			change_scene(StageList.start->next->data->GetString());
			secondStage = true;
			firstStage = false;
			
		}

		else //stage 1
		{
			change_scene(StageList.start->data->GetString());
			firstStage = true;
			secondStage = false;

		}

	}

	else if (secondStage)
	{
		//stage1
		if (DestinationStage1)
		{
			change_scene(StageList.start->data->GetString());
			firstStage = true;
			secondStage = false;
			
		}
		//stage2
		else
		{
			change_scene(StageList.start->next->data->GetString());
			firstStage = false;
			secondStage = true;
			
		}
	}


	
	loadSaveDataEntity();

	// --Timer reset ---
	//sceneTimer.Start();
	sceneTimer.LoadXtime(player->TimeAuxload);

	sceneTimer.changePausedtime(player->TimePausedSave);

	player->entitycoll->SetPos(player->position.x, player->position.y);

	return ret;
}

void j1Scene::setStandarEntityPosition(const char* map_name)
{
	player->CurrentAnimation = player->playerinfo.idleRight;
	// set entity position defined on tiled
	if (FirstStage == map_name)
	{
		player->Future_position.x = App->map->data.initpos.x;
		player->Future_position.y = App->map->data.initpos.y;
	}
	else
	{
		player->Future_position.x = App->map->data2.initpos.x;
		player->Future_position.y = App->map->data2.initpos.y;
	}

	// set colliders

	player->entitycoll = App->coll->AddCollider(player->entitycollrect, COLLIDER_TYPE::COLLIDER_PLAYER, App->entities);
	player->entitycoll->SetPos(player->position.x, player->position.y);

	// avtive and inactive

	player->active = true;
	player->dead = false;
}

void j1Scene::loadSaveDataEntity()
{
	p2SString tmp;
	tmp.create("save_game.xml");


	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(tmp.GetString());

	if (result != NULL)
	{
		LOG("Loading entity states State from %s...", tmp.GetString());

		root = data.child("game_state");

		App->entities->Load(root.child(App->entities->name.GetString()));

		data.reset();

		LOG("...finished loading");
		
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", tmp.GetString(), result.description());

	//sceneTimer.Loadstart(player->TimeAuxload);
}

void j1Scene::ONhover(j1UI_Element & element)
{
	ELEMENTS TYPE = element.GetType();

	switch (TYPE)
	{
	case ELEMENTS::PANEL:

		break;

	case ELEMENTS::BUTTON:
		element.Getrects()->current_rect = element.Getrects()->rect_hover;
		break;

	case ELEMENTS::LABEL:
		switch (element.Getparent()->GetType())
		{
		case ELEMENTS::BUTTON:

			break;

		case ELEMENTS::INPUT:

			break;

		}
		//element.ShapeLabel(element.GetTexts()->text2);
		break;

	case ELEMENTS::INPUT:

		break;
	}
}

void j1Scene::OFFhover(j1UI_Element & element)
{
	ELEMENTS TYPE = element.GetType();

	switch (TYPE)
	{
	case ELEMENTS::PANEL:

		break;

	case ELEMENTS::BUTTON:
		element.Getrects()->current_rect = element.Getrects()->rect_normal;
		break;

	case ELEMENTS::LABEL:
		switch (element.Getparent()->GetType())
		{
		case ELEMENTS::BUTTON:

			break;

		case ELEMENTS::INPUT:

			break;

		}
		//element.ShapeLabel(element.GetTexts()->text);
		break;

	case ELEMENTS::INPUT:

		break;
	}
}

void j1Scene::ONclick(j1UI_Element & element)
{
	ELEMENTS TYPE = element.GetType();

	App->audio->PlayFx(App->audio->buttonpopfx);

	switch (TYPE)
	{
	case ELEMENTS::PANEL:
		App->gui->focus = &element;
		break;

	case ELEMENTS::BUTTON:

		element.Getrects()->current_rect = element.Getrects()->rect_click;
		App->gui->focus = &element;
		break;

	case ELEMENTS::LABEL:
		switch (element.Getparent()->GetType())
		{
		case ELEMENTS::BUTTON:
			App->gui->focus = &element;
			break;

		case ELEMENTS::PANEL:
			App->gui->focus = &element;
			break;

		case ELEMENTS::INPUT:
			App->gui->focus = element.Getparent();
			break;

		}
		break;

	case ELEMENTS::INPUT:
		//App->gui->focus = &element;
		break;
	}
}

void j1Scene::OFFclick(j1UI_Element & element)
{
	ELEMENTS TYPE = element.GetType();

	switch (TYPE)
	{
	case ELEMENTS::PANEL:

		break;

	case ELEMENTS::BUTTON:
		element.Getrects()->current_rect = element.Getrects()->rect_normal;
		break;

	case ELEMENTS::LABEL:
		switch (element.Getparent()->GetType())
		{
		case ELEMENTS::BUTTON:

			break;

		case ELEMENTS::INPUT:

			break;

		}
		break;

	case ELEMENTS::INPUT:

		break;
	}
}

void j1Scene::ONdrag(j1UI_Element & element)
{
	ELEMENTS TYPE = element.GetType();

	switch (TYPE)
	{
	case ELEMENTS::PANEL:
		// Uncomment to enable panel dragging
		/*element.position.x = App->gui->mouse_pos.x - App->gui->drag_Ref.x;
		element.position.y = App->gui->mouse_pos.y - App->gui->drag_Ref.y;*/
		break;

	case ELEMENTS::BUTTON:
		// Uncomment to enable button dragging
		/*element.position.x = App->gui->mouse_pos.x - App->gui->drag_Ref.x;
		element.position.y = App->gui->mouse_pos.y - App->gui->drag_Ref.y;*/
		break;

	case ELEMENTS::SLIDER:

		parentindex = element.Getchildrencount();

		element.position.y = App->gui->mouse_pos.y - App->gui->drag_Ref.y;

		if (element.position.y > element.Getparent()->position.y + element.Getparent()->Getrects()->current_rect.h - element.Getrects()->current_rect.h / 2)
		{
			element.position.y = element.Getparent()->position.y + element.Getparent()->Getrects()->current_rect.h - element.Getrects()->current_rect.h / 2;
		}

		else if (element.position.y < element.Getparent()->position.y - element.Getrects()->current_rect.h / 2)
		{
			element.position.y = element.Getparent()->position.y - element.Getrects()->current_rect.h / 2;
		}

		if (parentindex == 0 && &element == App->gui->UIelements.At(11)->data || &element == App->gui->UIelements.At(30)->data)
		{
			App->audio->VolumeChanger_music = float(float(element.position.y + (float)element.Getrects()->current_rect.h / 2 - element.Getparent()->position.y) / element.Getparent()->Getrects()->current_rect.h);
			if (App->audio->VolumeChanger_music > 1)
				App->audio->VolumeChanger_music = 1;
			else if (App->audio->VolumeChanger_music < 0.1)
				App->audio->VolumeChanger_music = 0;
			LOG("Volume_changer: %f", App->audio->VolumeChanger_music);

			App->audio->ChangeVolume_music(App->audio->VolumeChanger_music);
		
		}
		else if (parentindex == 0)
		{
			App->audio->VolumeChanger_fx = float(float(element.position.y + (float)element.Getrects()->current_rect.h / 2 - element.Getparent()->position.y) / element.Getparent()->Getrects()->current_rect.h);
			if (App->audio->VolumeChanger_fx > 1)
				App->audio->VolumeChanger_fx = 1;
			else if (App->audio->VolumeChanger_fx < 0.1)
				App->audio->VolumeChanger_fx = 0;
			LOG("Volume_changer: %f", App->audio->VolumeChanger_fx);

			
			App->audio->ChangeVolume_fx(App->audio->VolumeChanger_fx);
		}
		else if (parentindex == 1)
		{
			element.Getchild(0)->position.y = element.Getparent()->Getrects()->logic_rect.y + ((element.Getrects()->logic_rect.y - element.Getparent()->Getrects()->logic_rect.y
				)* (element.Getchild(0)->Getrects()->rect_normal.h / element.Getparent()->Getrects()->logic_rect.h));
			//element.Getchild(0)->position.y = element.Getparent()->Getrects()->logic_rect.y + (element.Getrects()->logic_rect.y + element.Getrects()->logic_rect.h / 2 - element.Getparent()->Getrects()->logic_rect.y);
		}
		break;

	case ELEMENTS::LABEL:

		switch (element.Getparent()->GetType())
		{
		case ELEMENTS::BUTTON:

			break;

		case ELEMENTS::INPUT:

			break;

		}
		break;

	case ELEMENTS::INPUT:

		break;
	}
}

void j1Scene::ONFocus()
{
	if (App->gui->focus_index < App->gui->UIelements.count())
	{
		App->gui->focus = App->gui->UIelements.At(App->gui->focus_index)->data;
		App->gui->Colorize(*App->gui->focus->GetTexture(), 255, 120, 120, 25);
		if (App->gui->focus_index != 0)
			App->gui->DeColorize(*App->gui->UIelements.At(App->gui->focus_index)->prev->data->GetTexture());
		App->gui->focus_index++;
	}
	else
	{
		App->gui->DeColorize(*App->gui->UIelements.At(App->gui->focus_index - 1)->data->GetTexture());
		App->gui->focus_index = 0;
	}
}



bool j1Scene::Fade(int red, int green, int blue, float time)
{
	bool ret = false;

	fadeState = true;
	player->playerinfo.dontShow = fadeState;
	redAmount = red;
	blueAmount = blue;
	greenAmount = green;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);

		ret = true;
	}

	return ret;
}
