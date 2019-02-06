#ifndef __j1TIMER_H__
#define __j1TIMER_H__

#include "p2Defs.h"

class j1Timer
{
public:

	// Constructor
	j1Timer();

	void Start();
	uint32 Read() ;
	float ReadSec();
	 // getters
	uint32 startreturn();

	// --- timer save and load ---
	void LoadXtime(uint32 loaded);
	void ResetX();
	void changePausedtime(uint32 paused);
	void ResetPaused();
	uint32 getPausd();
	void ResetTotalPasedTime();
	// -- stop timer and get variables ---
	void Stop();
	uint32 getTotalTimeofPaused();
	
	

private:
	uint32	started_at;
	uint32	loadX =0;
	uint32 paused_time = 0;
	uint32 totalPausedTime = 0;
};

#endif //__j1TIMER_H__