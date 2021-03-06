#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Scene.h"
#include "j1Unit.h"
#include "j1EntityManager.h"
#include "j1MovementManager.h"

#include "Brofiler/Brofiler.h"

j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
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
		std::string* StageName = new std::string(stage.attribute("path").as_string());
		StageList.push_back(StageName);
	}

	return ret;
}

bool j1Scene::SetWalkabilityMap()
{
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
		App->pathfinding->SetMap(w, h, data);

	RELEASE_ARRAY(data);

	return true;
}

bool j1Scene::ChangeMap(int destination_map_id)
{
	App->map->CleanUp();

	if(App->map->Load(StageList.at(destination_map_id)->data()))
	{
		SetWalkabilityMap();
	}

	return true;
}

// Called before the first frame
bool j1Scene::Start()
{
	// --- Loading map ---

	if(App->map->Load(StageList.front()->data()) == true)
	{
		SetWalkabilityMap();
	}

	debug_tex = App->tex->Load("maps/path2.png");
	debug_tex2 = App->tex->Load("maps/path.png");

	return true;
}


void j1Scene::CreateEntityOnMouse(iPoint mouse_position)
{
	iPoint mouse_map = App->map->WorldToMap(mouse_position.x, mouse_position.y);

	if (App->pathfinding->IsWalkable(mouse_map) == true)
	{
		entity_info tmp;
		UnitInfo infotmp;

		// --- Create Entity info ---
		tmp.Size = { 24,24 };
		tmp.Speed = 100;
		tmp.position = { (float)mouse_position.x,(float)mouse_position.y };

		App->manager->CreateEntity(entity_type::UNIT, tmp, infotmp);
	}
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	BROFILER_CATEGORY("Scene_Pre_Update", Profiler::Color::BlanchedAlmond);

	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	mouse_pos = App->render->ScreenToWorld(x, y);

	// --- If We press the middle mouse button, create a unit on mouse position ---
	if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_DOWN)
	{
		CreateEntityOnMouse(mouse_pos);
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene_Update", Profiler::Color::Crimson);

	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		ChangeMap(1);

	//Make the camera movement independent of framerate
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += ceil(350.0*dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= ceil(350.0*dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += ceil(350.0*dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= ceil(350.0*dt);

	App->map->Draw();

	int x, y;

	// Debug pathfinding ------------------------------
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x, p.y);

	// --- Calling Selection Rectangle Tool ---

	RectangleSelection();

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Scene_Post_Update", Profiler::Color::Aquamarine);

	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	std::vector<std::string*>().swap(StageList);

	return true;
}

void j1Scene::RectangleSelection()
{
	rectangle_width = mouse_pos.x - rectangle_origin.x;
	rectangle_height = mouse_pos.y - rectangle_origin.y;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		rectangle_origin = mouse_pos;

	else if (std::abs(rectangle_width) >= RECT_MIN_AREA && std::abs(rectangle_height) >= RECT_MIN_AREA && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		// --- Rectangle size ---
		int width = mouse_pos.x - rectangle_origin.x;
		int height = mouse_pos.y - rectangle_origin.y;

		// --- Draw Rectangle ---
		SDL_Rect SRect = { rectangle_origin.x, rectangle_origin.y, width, height };
		App->render->DrawQuad(SRect, 255, 255, 255, 255, false);

		// --- Once we get to the negative side of SRect numbers must be adjusted ---
		if (width < 0) {
			SRect.x = mouse_pos.x;
			SRect.w *= -1;
		}
		if (height < 0) {
			SRect.y = mouse_pos.y;
			SRect.h *= -1;
		}

		// --- Check for Units in the rectangle, select them ---

		App->Mmanager->SelectEntities_inRect(SRect);

		//LOG("rect is x%i y%i w%i h%i", SRect.x, SRect.y, SRect.w, SRect.h);
	}

	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
		App->Mmanager->CreateGroup();
	
}

