#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include <vector>

struct SDL_Texture;
class j1Player;

#define RECT_MIN_AREA 5

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

	void RectangleSelection();

public:

	// Set new Walkability map
	bool SetWalkabilityMap();

	// Change current map
	bool ChangeMap(int destination_map_id);

	iPoint mouse_pos = { 0,0 };
	iPoint rectangle_origin = { 0,0 };
	int rectangle_width = 0;
	int rectangle_height = 0;
	SDL_Texture* debug_tex2;

private:
	SDL_Texture* debug_tex;

	std::vector <std::string*> StageList;

	//Entities on map
	j1Player*           player = nullptr;
	j1Player*           player2 = nullptr;
	j1Player*           player3 = nullptr;
	j1Player*           player4 = nullptr;
};

#endif // __j1SCENE_H__