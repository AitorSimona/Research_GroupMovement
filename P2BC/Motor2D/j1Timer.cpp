// ----------------------------------------------------
// j1Timer.cpp
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "j1Timer.h"
#include "SDL\include\SDL_timer.h"

// ---------------------------------------------
j1Timer::j1Timer()
{
	Start();
}

// ---------------------------------------------
void j1Timer::Start()
{
	started_at = SDL_GetTicks();
}

// ---------------------------------------------
uint32 j1Timer::Read() 
{
	if (SDL_GetTicks() <= started_at + loadX + paused_time)
	{
		loadX = 0;
		paused_time = 0;
	}
	return SDL_GetTicks() - started_at - loadX- paused_time;
}

// ---------------------------------------------
float j1Timer::ReadSec() 
{

	if (SDL_GetTicks() <= started_at + loadX + paused_time)
	{
		loadX = 0;
		paused_time = 0;
	}
		return float(SDL_GetTicks() - started_at - loadX - paused_time) / 1000.0f;
	
}

// ---------------------------------------------
uint32 j1Timer::startreturn() 
{
	return started_at;
}

// ---------------------------------------------
void j1Timer::LoadXtime(uint32 loaded)
{
	uint lol = SDL_GetTicks() - started_at;
	loadX = lol - loaded;
}

void j1Timer::ResetX()
{
	loadX = 0;
}

void j1Timer::changePausedtime(uint32 paused)
{
	paused_time = paused;
}

void j1Timer::ResetPaused()
{
	paused_time = 0;
}

void j1Timer::Stop()
{
	totalPausedTime += Read() ;
}

uint32 j1Timer::getTotalTimeofPaused()
{
	return totalPausedTime;
}

uint32 j1Timer::getPausd()
{
	return paused_time;
}

void j1Timer::ResetTotalPasedTime()
{
	totalPausedTime = 0;
}
