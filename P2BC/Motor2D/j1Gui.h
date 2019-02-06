#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "j1UI_Element.h"
#include "j1Button.h"
#include "j1Label.h"

#define CURSOR_WIDTH 2
// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	friend class j1Button;
	friend class j1Scene;

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	SDL_Texture* GetAtlas() const;

	SDL_Texture* CreateImage(const char* path);

	void DeployUI(pugi::xml_node &UIconfig);

	// --- UI ELEMENT ---

	// --- BUTTON --- // --- PANEL ---
	j1UI_Element* CreateButton(ButtonInfo &Data);
	ButtonInfo FillButton(pugi::xml_node &UIconfig);
	// --- LABEL ---
	j1UI_Element* CreateLabel(Text &Data);
	Text FillLabel(pugi::xml_node &UIconfig);


	bool RecursiveOnDrag(p2List_item <j1UI_Element*> * item);
	bool RecursiveOnHover(p2List_item <j1UI_Element*> * item);
	bool RecursiveOnClick(p2List_item <j1UI_Element*> * item);


	// --- Utilities ---
	bool isInbound(SDL_Rect &rect);
	bool isClicked(SDL_Rect &rect);
	void DebugDraw();

	bool Colorize(SDL_Texture& tex, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const;
	void DeColorize(SDL_Texture& tex) const;

private:

	p2List <j1UI_Element*> UIelements;;
	j1UI_Element* focus = nullptr;
	int focus_index = 0;

	SDL_Texture* atlas = nullptr;
	SDL_Texture* logo = nullptr;

	p2SString atlas_file_name;
	p2SString logo_file_name;

	iPoint mouse_pos = { 0,0 };
	iPoint click_pos = { 0,0 };
	iPoint drag_Ref = { 0,0 };
	uint first_children = 0;

	bool debug = false;
	bool skip_drag = false;
};

#endif // __j1GUI_H__
