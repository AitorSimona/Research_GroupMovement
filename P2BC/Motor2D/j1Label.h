#ifndef __j1Label_H__
#define __j1Label_H__

#include "j1UI_Element.h"

enum class Text_Position
{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	MIDDLE,

	NONE
};

struct Text
{
	textbox texts;
	SDL_Color  color;
	SDL_Texture* tex = nullptr;
	Text_Position Place;
	Booleans bools;
	ELEMENTS type;
	Buttonrects rects;
	iPoint position;

	int parent_id = -1;
};

class j1Label : public j1UI_Element
{
public:

	j1Label(Text Data) : Data(Data) {}

	~j1Label() {}

	void FixedUpdate();

	// --- Data Getters ---
	ELEMENTS GetType() override;
	Booleans * GetBooleans() override;
	Buttonrects* Getrects() override;
	textbox * GetTexts() override;
	SDL_Texture* GetTexture();

	//--- Label placing functions ---
	inline void PlaceAtTop();
	inline void PlaceAtBottom();
	inline void PlaceAtLeft();
	inline void PlaceAtRight();
	inline void PlaceAtMiddle();

	// --- Utilities ---
	void ShapeLabel(const char *text) override;

private:
	Text Data;
	uint start = 0;
	SDL_Rect blittext = { 0,0,0,0 };
};


#endif // __j1Label_H__

