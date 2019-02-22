#ifndef __j1Player_H__
#define __j1Player_H__

#include "j1Module.h"
#include "p2Point.h"
#include "j1Entity.h"

struct SDL_Texture;

class j1Player :public j1Entity
{
public:

	j1Player();
	~j1Player();

	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	// Called each loop iteration
	void FixedUpdate(float dt);

	// Called each logic iteration
	void LogicUpdate(float dt);

	void UpdateEntityMovement(float dt);


public:
	// --- NEW APPROACH VARIABLES ---

	float Accumulative_pos_Right = 0;
	float Accumulative_pos_Left = 0;
	float Accumulative_pos_Up = 0;
	float Accumulative_pos_Down = 0;
	fPoint Future_position = { 0,0 };
	bool				do_logic = false;
	int					logic_updates_per_second = 0;
	float				update_s_cycle = 0;
	float				accumulated_time = 0;
	int frame_count = 0;

private:


};

#endif // __j1Player_H__
