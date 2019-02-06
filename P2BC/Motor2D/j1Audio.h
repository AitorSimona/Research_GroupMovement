#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	//Modify Volume

	void ChangeVolume_music(float value);
	void ChangeVolume_fx(float value);

	//Load & Save audio data

	bool Save(pugi::xml_node&config) const;
	bool Load(pugi::xml_node&config);

	p2SString musicfolder =nullptr;
	p2SString fxfolder = nullptr;
	p2List<p2SString*>	SongNamesList;

	float VolumeChanger_music=0.0;
	float VolumeChanger_fx=0.0;

	bool testbool = false;
	p2SString fxDeath = nullptr;
	p2SString fxJump = nullptr;
	p2SString fxdoubleJump = nullptr;
	p2SString fxbuttonpop = nullptr;
	p2SString fxorb = nullptr;
	p2SString fxenemydeath = nullptr;

	unsigned int deathfx = NULL;
	unsigned int jumpfx = NULL;
	unsigned int doublejumpfx = NULL;
	unsigned int buttonpopfx = NULL;
	unsigned int orbfx = NULL;
	unsigned int enemydeathfx = NULL;

private:

	_Mix_Music*			music = NULL;
	p2List<Mix_Chunk*>	fx;
};

#endif // __j1AUDIO_H__