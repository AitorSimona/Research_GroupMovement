#ifndef __j1UI_Element_H__
#define __j1UI_Element_H__

#include "SDL/include/SDL.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2SString.h"

enum class ELEMENTS
{
	PANEL,
	BUTTON,
	LABEL,
	INPUT,
	SLIDER,

	NONE
};

struct Booleans
{
	bool hovering = false;
	bool clicking = false;
	bool dragging = false;
};

struct Buttonrects
{
	SDL_Rect logic_rect = { 0,0,0,0 };
	SDL_Rect current_rect = { 0,0,0,0 };
	SDL_Rect rect_hover = { 0,0,0,0 };
	SDL_Rect rect_click = { 0,0,0,0 };
	SDL_Rect rect_normal = { 0,0,0,0 };
};

struct textbox
{
	p2SString current_text = nullptr;
	const char* text = nullptr;
	const char* text2 = nullptr;
};

class j1UI_Element
{
	friend class j1Gui;
public:
	j1UI_Element();

	virtual ~j1UI_Element();

	virtual void FixedUpdate() {}

	virtual ELEMENTS GetType();

	virtual Buttonrects* Getrects();

	virtual Booleans * GetBooleans();

	virtual SDL_Texture* GetTexture();

	// --- Label exclusive ---

	virtual textbox * GetTexts();

	virtual void ShapeLabel(const char *text) {}

	j1UI_Element* Getparent() const { return parent; }

	int Getchildrencount() const { return children.count(); }

	j1UI_Element* Getchild(int index) { return children.At(index)->data; }

	bool* GetActive() { return &active; }

public:
	iPoint position;

protected:
	p2List <j1UI_Element*> children;
	j1UI_Element* parent = nullptr;
	bool active = true;
};


#endif // __j1UI_Element_H__
