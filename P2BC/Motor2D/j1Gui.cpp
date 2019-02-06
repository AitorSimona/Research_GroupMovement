#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "Brofiler\Brofiler.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");
	logo_file_name = conf.child("logo").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = CreateImage(atlas_file_name.GetString());
	logo = CreateImage(logo_file_name.GetString());

	if (atlas == nullptr)
		return false;

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	BROFILER_CATEGORY("GUI_Pre_Update", Profiler::Color::Gold);

	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	if (App->input->GetMouseButtonDown(1) == KEY_DOWN)
	{
		click_pos = mouse_pos;
	}
	else if (App->input->GetMouseButtonDown(1) == KEY_UP)
	{
		click_pos = { -1,-1 };
		skip_drag = false;
	}

	//if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
	//{
	//	App->scene->ONFocus();
	//}

	//if (focus != nullptr && focus->GetType() == ELEMENTS::INPUT && SDL_IsTextInputActive() == false)
	//{
	//	App->input->StartTextInput();
	//}

	//else if (focus != nullptr && focus->GetType() != ELEMENTS::INPUT && SDL_IsTextInputActive() == true)
	//{
	//	App->input->StopTextInput();
	//}

	//if (SDL_IsTextInputActive() == true)
	//{
	//	if (App->input->inputtext.Length() != 0)
	//	{
	//		focus->children.start->data->GetTexts()->current_text += App->input->inputtext.GetString();
	//		LOG("string is: %s", focus->children.start->data->GetTexts()->current_text.GetString());
	//		focus->children.start->data->ShapeLabel(focus->children.start->data->GetTexts()->current_text.GetString());
	//	}
	//}

	p2List_item <j1UI_Element*> * item = UIelements.At(first_children - 1);

	bool ret = false;

	while (item)
	{
		ret = false;

		if (App->input->GetMouseButtonDown(1) == KEY_DOWN)
			ret = RecursiveOnClick(item);

			/*if (ret)
				break;*/

			if (App->input->GetMouseButtonDown(1) == KEY_DOWN)
				ret = RecursiveOnDrag(item);

			if (ret)
				break;
			if (!ret)
				RecursiveOnHover(item);

		item = item->prev;
	}

	item = UIelements.end;

	while (item)
	{
			if (App->input->GetMouseButtonDown(1) == KEY_UP)
				item->data->GetBooleans()->dragging = false;

			else
			{
				if (item->data->GetBooleans()->dragging)
				{
					App->scene->ONdrag(*item->data);
				}
			}

		item = item->prev;
	}

	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate(float dt)
{
	BROFILER_CATEGORY("GUI_Post_Update", Profiler::Color::FloralWhite);

	p2List_item <j1UI_Element*> * item = UIelements.start;

	while (item)
	{
		item->data->FixedUpdate();

		item = item->next;
	}

	DebugDraw();

	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	p2List_item <j1UI_Element*> * item = UIelements.start;

	while (item)
	{
		item->data->children.clear();

		RELEASE(item->data);
		item = item->next;
	}

	UIelements.clear();

	return true;
}

// const getter for atlas
SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

SDL_Texture * j1Gui::CreateImage(const char * path)
{
	SDL_Texture * tex = App->tex->Load(path);
	return tex;
}

void j1Gui::DeployUI(pugi::xml_node &UIconfig)
{
	BROFILER_CATEGORY("GUI_Deploy_UI", Profiler::Color::GhostWhite);

	UIconfig = UIconfig.child("Element");
	uint elem_type;

	while (UIconfig)
	{
		elem_type = UIconfig.child("type").attribute("id").as_uint();

		if (elem_type == static_cast <uint> (ELEMENTS::PANEL))
		{
			CreateButton(FillButton(UIconfig));
		}

		if (elem_type == static_cast <uint> (ELEMENTS::BUTTON))
		{
			CreateButton(FillButton(UIconfig));
		}

		if (elem_type == static_cast <uint> (ELEMENTS::LABEL))
		{
			CreateLabel(FillLabel(UIconfig));
		}

		if (elem_type == static_cast <uint> (ELEMENTS::INPUT))
		{
			CreateButton(FillButton(UIconfig));
		}

		if (elem_type == static_cast <uint> (ELEMENTS::SLIDER))
		{
			CreateButton(FillButton(UIconfig));
		}

		UIconfig = UIconfig.next_sibling();
	}

	//LOG("first children: %u", first_children);
}

ButtonInfo j1Gui::FillButton(pugi::xml_node & UIconfig)
{
	ButtonInfo Data;

	// --- Parent ID ---
	Data.parent_id = UIconfig.child("parent").attribute("id").as_int();

	if (Data.parent_id == -1)
		first_children++;

	Data.type = (ELEMENTS)UIconfig.child("type").attribute("id").as_int();

	// --- Position ---
	Data.position.x = UIconfig.child("position").attribute("x").as_int();
	Data.position.y = UIconfig.child("position").attribute("y").as_int();

	// --- Texture ---
	p2SString path = UIconfig.child("Texture").attribute("path").as_string();

	if (path == atlas_file_name)
		Data.tex = atlas;

	else if (path == logo_file_name)
		Data.tex = logo;

	// --- Rectangles ---
	Data.rects.logic_rect.x = Data.position.x;
	Data.rects.logic_rect.y = Data.position.y;
	Data.rects.logic_rect.w = UIconfig.child("logic_rect").attribute("w").as_int();
	Data.rects.logic_rect.h = UIconfig.child("logic_rect").attribute("h").as_int();

	Data.rects.rect_normal.x = UIconfig.child("normal_rect").attribute("x").as_int();
	Data.rects.rect_normal.y = UIconfig.child("normal_rect").attribute("y").as_int();
	Data.rects.rect_normal.w = UIconfig.child("normal_rect").attribute("w").as_int();
	Data.rects.rect_normal.h = UIconfig.child("normal_rect").attribute("h").as_int();
	Data.rects.current_rect = Data.rects.rect_normal;

	Data.rects.rect_hover.x = UIconfig.child("hover_rect").attribute("x").as_int();
	Data.rects.rect_hover.y = UIconfig.child("hover_rect").attribute("y").as_int();
	Data.rects.rect_hover.w = UIconfig.child("hover_rect").attribute("w").as_int();
	Data.rects.rect_hover.h = UIconfig.child("hover_rect").attribute("h").as_int();

	Data.rects.rect_click.x = UIconfig.child("click_rect").attribute("x").as_int();
	Data.rects.rect_click.y = UIconfig.child("click_rect").attribute("y").as_int();
	Data.rects.rect_click.w = UIconfig.child("click_rect").attribute("w").as_int();
	Data.rects.rect_click.h = UIconfig.child("click_rect").attribute("h").as_int();

	return Data;
}

Text j1Gui::FillLabel(pugi::xml_node & UIconfig)
{
	Text Data;

	// --- Parent ID ---
	Data.parent_id = UIconfig.child("parent").attribute("id").as_int();

	if (Data.parent_id == -1)
		first_children++;

	Data.type = (ELEMENTS)UIconfig.child("type").attribute("id").as_int();

	// --- Position ---
	Data.position.x = UIconfig.child("position").attribute("x").as_int();
	Data.position.y = UIconfig.child("position").attribute("y").as_int();
	Data.Place = (Text_Position)UIconfig.child("place").attribute("id").as_uint();

	// --- Colour ---
	Data.color.r = UIconfig.child("color").attribute("r").as_uint();
	Data.color.g = UIconfig.child("color").attribute("g").as_uint();
	Data.color.b = UIconfig.child("color").attribute("b").as_uint();
	Data.color.a = UIconfig.child("color").attribute("a").as_uint();

	// --- Texts ---
	Data.texts.text = UIconfig.child("textA").attribute("value").as_string();
	Data.texts.text2 = UIconfig.child("textB").attribute("value").as_string();
	Data.texts.current_text = Data.texts.text;
	// --- Rectangles ---
	Data.tex = App->font->Print(Data.texts.text, Data.color, App->font->default);
	App->font->CalcSize(Data.texts.text, Data.rects.rect_normal.w, Data.rects.rect_normal.h, App->font->default);

	if (Data.rects.rect_normal.w > 250)
	{
		Data.rects.rect_normal.h *= Data.rects.rect_normal.w / 250;

		//Data.rects.rect_normal.h = 324;
		Data.rects.rect_normal.w = 250;
	}

	Data.rects.logic_rect = { 0 , 0 , Data.rects.rect_normal.w ,324};
	Data.texts.current_text = Data.texts.text;

	if (Data.rects.rect_normal.w < 250)
		Data.rects.logic_rect.h = Data.rects.rect_normal.h;

	return Data;
}

bool j1Gui::RecursiveOnDrag(p2List_item<j1UI_Element*>* item)
{
	bool ret = false;

	ret = true;

	bool child_drag = false;

	if (item->data->children.count() > 0)
	{
		p2List_item <j1UI_Element*> * item_child = item->data->children.end;

		while (item_child)
		{
			child_drag = RecursiveOnDrag(item_child);

			if (child_drag == true)
				break;

			item_child = item_child->prev;
		}
	}

	if (child_drag == false && isClicked(item->data->Getrects()->logic_rect) && *item->data->GetActive()==true)
	{
		item->data->GetBooleans()->dragging = true;
		drag_Ref.x = click_pos.x - item->data->position.x;
		drag_Ref.y = click_pos.y - item->data->position.y;
	}
	else if (child_drag == false)
	{
		ret = false;

		if (item->data->GetBooleans()->dragging = true)
		{
			item->data->GetBooleans()->dragging = false;
		}
	}


	return ret;
}

bool j1Gui::RecursiveOnHover(p2List_item<j1UI_Element*>* item)
{
	bool ret = false;

	ret = true;

	bool child_hover = false;

	if (item->data->children.count() > 0)
	{
		p2List_item <j1UI_Element*> * item_child = item->data->children.end;

		while (item_child)
		{
			child_hover = RecursiveOnHover(item_child);

			if (child_hover == true)
				break;

			item_child = item_child->prev;
		}
	}

	if (isInbound(item->data->Getrects()->logic_rect))
	{
		if (child_hover == false && *item->data->GetActive() == true)
		{
			item->data->GetBooleans()->hovering = true;
			App->scene->ONhover(*item->data);
		}
	}
	else
	{
		ret = false;

		if (item->data->GetBooleans()->hovering = true)
		{
			item->data->GetBooleans()->hovering = false;
			App->scene->OFFhover(*item->data);
		}
	}


	return ret;
}

bool j1Gui::RecursiveOnClick(p2List_item<j1UI_Element*>* item)
{
	bool ret = false;

	ret = true;

	bool child_click = false;

	if (item->data->children.count() > 0)
	{
		p2List_item <j1UI_Element*> * item_child = item->data->children.end;

		while (item_child)
		{
			child_click = RecursiveOnClick(item_child);

			if (child_click == true)
				break;

			item_child = item_child->prev;
		}
	}

	if (child_click == false && isClicked(item->data->Getrects()->logic_rect) && *item->data->GetActive() == true)
	{
		item->data->GetBooleans()->clicking = true;
		App->scene->ONclick(*item->data);
	}
	else if (child_click == false)
	{
		ret = false;

		if (item->data->GetBooleans()->clicking == true)
		{
			item->data->GetBooleans()->clicking = false;
			App->scene->OFFclick(*item->data);
		}
	}


	return ret;
}

j1UI_Element * j1Gui::CreateLabel(Text & Data)
{
	j1UI_Element *label = (j1UI_Element*) new j1Label(Data);

	if (Data.parent_id != -1)
	{
		label->parent = UIelements.At(Data.parent_id)->data;
		UIelements.At(Data.parent_id)->data->children.add(label);
	}

	UIelements.add(label);

	// --- Positioning label ---
	label->position = Data.position;

	return label;
}

j1UI_Element * j1Gui::CreateButton(ButtonInfo &Data)
{
	j1UI_Element *button = (j1UI_Element*) new j1Button(Data);

	if (Data.parent_id != -1)
	{
		button->parent = UIelements.At(Data.parent_id)->data;
		UIelements.At(Data.parent_id)->data->children.add(button);
	}

	UIelements.add(button);

	// --- Positioning button ---
	button->position = Data.position;

	return button;
}

bool j1Gui::isInbound(SDL_Rect &rect)
{
	return (rect.x < mouse_pos.x &&
		rect.x + rect.w > mouse_pos.x &&
		rect.y < mouse_pos.y  &&
		rect.h + rect.y > mouse_pos.y);
}

bool j1Gui::isClicked(SDL_Rect & rect)
{
	return (rect.x < click_pos.x &&
		rect.x + rect.w > click_pos.x &&
		rect.y < click_pos.y  &&
		rect.h + rect.y > click_pos.y);
}

void j1Gui::DebugDraw()
{

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 50;

	p2List_item <j1UI_Element*> * item = UIelements.end;

	while (item != NULL)
	{
		if (*item->data->GetActive() == true)
		{
			switch (item->data->GetType())
			{
			case ELEMENTS::LABEL: // white
				App->render->DrawQuad(item->data->Getrects()->logic_rect, 255, 255, 255, alpha, false, false);
				break;
			case ELEMENTS::BUTTON: // white
				App->render->DrawQuad(item->data->Getrects()->logic_rect, 255, 255, 255, alpha, false, false);
				break;
			case ELEMENTS::PANEL: // white
				App->render->DrawQuad(item->data->Getrects()->logic_rect, 255, 255, 255, alpha, false, false);
				break;
			}
		}
		item = item->prev;
	}

	//if (focus != nullptr)
	//App->render->DrawQuad(SDL_Rect{ focus->Getrects()->logic_rect.x ,focus->Getrects()->logic_rect.y,1,focus->Getrects()->logic_rect.h }, 255, 255, 255, 255, true, false);

}

void j1Gui::DeColorize(SDL_Texture & tex) const
{
	SDL_SetTextureColorMod(&tex, 255, 255, 255);
}

bool j1Gui::Colorize(SDL_Texture& tex, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
{
	bool ret = true;

	SDL_SetTextureBlendMode(&tex, SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(&tex, r, g, b);

	return ret;
}

// class Gui ---------------------------------------------------

