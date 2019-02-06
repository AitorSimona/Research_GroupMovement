#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Timer.h"

struct SDL_Texture;
struct SDL_Rect;
class j1Player;
class j1Slime;
class j1Bat;
class j1Orb;
class j1Button;
class j1UI_Element;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	//call to save stage info
	bool Save(pugi::xml_node&config) const;

	//call to load file
	bool Load(pugi::xml_node&config);

	bool change_scene(const char* map_name);

	void setStandarEntityPosition(const char* map_name);

	void loadSaveDataEntity();

	// --- UI ---

	void ONhover(j1UI_Element & element);
	void OFFhover(j1UI_Element & element);
	void ONclick(j1UI_Element & element);
	void OFFclick(j1UI_Element & element);
	void ONdrag(j1UI_Element & element);

	void ONFocus();

public:

	// --- UI ---
	float Volume_changer = 0.5f;
	int parentindex = 0;
	bool Activate_Ingamemenu = false;
	bool Activate_MainMenu = true;
	bool Activate_InGameSettings = false;
	bool Activate_MainMenuSettings = false;
	bool Activate_Credits = false;
	bool Activate_HUD = false;
	bool Existant_SaveGame = false;

	p2SString MenuMusic;
	bool playing_menu = false;
	bool playing_stage1 = false;
	bool playing_stage2 = false;
	p2SString stage1Music;
	p2SString stage2Music;


	const char * scorestring = nullptr;
	int score_sum = 0;
	std::string auxstr;

	bool no_continue = false;
	//---------------

	p2List<p2SString*> StageList;
	p2SString FirstStage;
	
	bool firstStage = true;
	bool secondStage = false;
	iPoint camera1;
	iPoint camera2;

	bool DestinationStage1 = false;
	bool DestinationStage2 = false;

	//Entities on map
	j1Player*           player = nullptr;
	j1Slime*			slime = nullptr;
	j1Slime*			slime2 = nullptr;
	j1Bat*				bat = nullptr;
	j1Bat*				bat2 = nullptr;
	j1Orb*				orb = nullptr;
	j1Orb*				orb2 = nullptr;
	j1Orb*				orb3 = nullptr;

	//variables used on module collision
	int areaofcollision = 0;

	//variables for save and load
	mutable int xSlime = 0;
	mutable int ySlime = 0;

	mutable int xSlime2 = 0;
	mutable int ySlime2 = 0;

	mutable	int xBat = 0;
	mutable int yBat = 0;

	mutable int xBat2 = 0;
	mutable int yBat2 = 0;

	mutable bool orbTouched = false;
	mutable bool orb2Touched = false;
	mutable bool orb3Touched = false;

	// -- Timer --
	j1Timer sceneTimer;

	uint32 timeWhenPaused=0;

	j1Timer timeBeingPaused;

	uint32 timeAccumulated=0; 

	bool taketime = false;

	private:
	p2SString map_name=nullptr;
	SDL_Rect debug_Tex_rect = { 96,0,16,16 };

	// -- Fade from scene to scene -- 
	bool fadeState = false;

	bool Fade( int red, int green, int blue, float time = 2.0f);
	
	private:
		enum fade_step
		{
			none,
			fade_to,
			fade_from
		} current_step = fade_step::none;
		Uint32 start_time = 0;
		Uint32 total_time = 0;
		SDL_Rect screen;
		uint window_h = 0;
		uint window_w = 0;
		uint scale = 0;
		int redAmount = 0;
		int greenAmount = 0;
		int blueAmount = 0;

};

#endif // __j1SCENE_H__