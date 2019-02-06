#ifndef __j1Button_H__
#define __j1Button_H__

#include "j1UI_Element.h"

struct ButtonInfo
{
	Buttonrects rects;
	iPoint position;
	SDL_Texture* tex;
	Booleans bools;
	ELEMENTS type;

	int parent_id = -1;
};

class j1Button : public j1UI_Element
{
public:

	j1Button(ButtonInfo Data) : Data(Data) {}

	~j1Button() {}

	void FixedUpdate();

	// --- Data Getters ---
	ELEMENTS GetType() override;
	Buttonrects* Getrects() override;
	Booleans * GetBooleans() override;
	SDL_Texture* GetTexture();

private:
	ButtonInfo Data;
};


#endif // __j1Button_H__

