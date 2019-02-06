#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Audio.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

j1Audio::j1Audio() : j1Module()
{
	music = NULL;
	name.create("audio");
}

// Destructor
j1Audio::~j1Audio()
{}

// Called before render is available
bool j1Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	
	//Path names for volume modifier's nodes

	musicfolder = config.child("music").child_value("folder");
	fxfolder = config.child("fx").child_value("folder");

	VolumeChanger_music = config.child("music").child("VolumeChanger_music").attribute("value").as_float();
	VolumeChanger_fx = config.child("fx").child("VolumeChanger_fx").attribute("value").as_float();

	fxDeath = config.child("fx").child("sound").attribute("death").as_string();
	fxJump = config.child("fx").child("sound").attribute("jump").as_string();
	fxdoubleJump = config.child("fx").child("sound").attribute("doubleJump").as_string();
	fxbuttonpop = config.child("fx").child("sound").attribute("buttonpop").as_string();
	fxorb = config.child("fx").child("sound").attribute("orb").as_string();
	fxenemydeath = config.child("fx").child("sound").attribute("enemydeath").as_string();

	p2SString deathSound ("%s%s", fxfolder.GetString(), fxDeath.GetString());
	deathfx = LoadFx(deathSound.GetString());

	p2SString enemydeathSound("%s%s", fxfolder.GetString(), fxenemydeath.GetString());
	enemydeathfx = LoadFx(enemydeathSound.GetString());

	p2SString jumpSound("%s%s", fxfolder.GetString(), fxJump.GetString());
	jumpfx = LoadFx(jumpSound.GetString());

	p2SString doublejumpSound("%s%s", fxfolder.GetString(), fxdoubleJump.GetString());
	doublejumpfx = LoadFx(doublejumpSound.GetString());

	p2SString buttonpopSound("%s%s", fxfolder.GetString(), fxbuttonpop.GetString());
	buttonpopfx = LoadFx(buttonpopSound.GetString());

	p2SString orbSound("%s%s", fxfolder.GetString(), fxorb.GetString());
	orbfx = LoadFx(orbSound.GetString());

	pugi::xml_node Music;
	for (Music = config.child("music").child("song"); Music && ret; Music = Music.next_sibling("song"))
	{
		p2SString* SongName = new p2SString();

		SongName->create(Music.attribute("name").as_string());
		SongNamesList.add(SongName);
	}

	return ret;
}

// Called before quitting
bool j1Audio::CleanUp()
{
	if(!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}

	p2List_item<Mix_Chunk*>* item;
	item = fx.start;
		
	while (item != NULL)
	{
		Mix_FreeChunk(item->data);
		item = item->next;
	}
	fx.clear();


	p2List_item<p2SString*>* item2;
	item2 = SongNamesList.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	SongNamesList.clear();

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool j1Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	Mix_VolumeMusic(SDL_MIX_MAXVOLUME*VolumeChanger_music);

	if(!active)
		return false;

	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{

			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int j1Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if(!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.add(chunk);
		ret = fx.count();
	}

	return ret;
}

// Play WAV
bool j1Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if(!active)
		return false;

	if(id > 0 && id <= fx.count())
	{
		Mix_VolumeChunk(fx[id - 1], SDL_MIX_MAXVOLUME * VolumeChanger_fx);
		Mix_PlayChannel(-1, fx[id - 1], repeat);
	}

	return ret;
}

void j1Audio::ChangeVolume_music(float value)
{
	// value is an int from 0 to a 100
	value = value / 100;

	VolumeChanger_music += value;

	//We make sure we use a number from 0 to 1
	if (VolumeChanger_music < 0)
		VolumeChanger_music = 0;

	if (VolumeChanger_music > 1)
		VolumeChanger_music = 1;

	Mix_VolumeMusic(128 * VolumeChanger_music);

}

void j1Audio::ChangeVolume_fx(float value)
{
	// value is an int from 0 to a 100, we need it to be from 0 to 1
	value = value / 100;

	VolumeChanger_fx += value;

	//We make sure we use a number from 0 to 1
	if (VolumeChanger_fx < 0)
		VolumeChanger_fx = 0;

	if (VolumeChanger_fx > 1)
		VolumeChanger_fx = 1;

	p2List_item <Mix_Chunk*> *chunkitem;

	for (chunkitem = fx.start; chunkitem != NULL; chunkitem = chunkitem->next)
	{
		Mix_VolumeChunk(chunkitem->data, 128 * VolumeChanger_fx);
	}

}

bool j1Audio::Save(pugi::xml_node &config) const
{
	bool ret = true;

	config.append_child("VolumeChanger_music").append_attribute("value") = VolumeChanger_music;
	config.append_child("VolumeChanger_fx").append_attribute("value") = VolumeChanger_fx;


	return ret;
}

bool j1Audio::Load(pugi::xml_node &config)
{
	bool ret = true;

	VolumeChanger_music = config.child("VolumeChanger_music").attribute("value").as_float();

	VolumeChanger_fx = config.child("VolumeChanger_fx").attribute("value").as_float();

	if (testbool == true)
		LOG("nice");

	ret = Mix_VolumeMusic(128 * VolumeChanger_music);

	p2List_item <Mix_Chunk*> *chunkitem;

	for (chunkitem = fx.start; chunkitem != NULL && ret != NULL; chunkitem = chunkitem->next)
	{
		ret = Mix_VolumeChunk(chunkitem->data, 128 * VolumeChanger_fx);
	}

	return ret;
}
