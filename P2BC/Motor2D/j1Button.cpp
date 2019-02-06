#include "j1Button.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Gui.h"

void j1Button::FixedUpdate()
{
	//// --- Controlling deactivated elements ---
	//if (parent == nullptr && active == false)
	//	return;
	//else if (parent != nullptr && *parent->GetActive() == false)
	//{
	//	active = false;
	//	return;
	//}

	//active = true;

	if (!active)
		return;

	if (this->parent != nullptr)
	{
		if (!this->GetBooleans()->dragging)
		{
			this->position.x = parent->position.x + Data.position.x;
			this->position.y = parent->position.y + Data.position.y;
		}
		else
		{
			Data.position.x = this->position.x - parent->position.x;
			Data.position.y = this->position.y - parent->position.y;
		}
	}

	this->Data.rects.logic_rect.x = position.x;
	this->Data.rects.logic_rect.y = position.y;

	if (this == App->gui->focus)
	{
		App->gui->Colorize(*Data.tex, 255, 120, 120, 25);
	}

	App->render->Blit(Data.tex, this->position.x, this->position.y, &Data.rects.current_rect, false);

	if (this == App->gui->focus)
	{
		App->gui->DeColorize(*Data.tex);
	}
}

ELEMENTS j1Button::GetType()
{
	return Data.type;
}

Buttonrects * j1Button::Getrects()
{
	return &Data.rects;
}

Booleans * j1Button::GetBooleans()
{
	return &Data.bools;
}

SDL_Texture * j1Button::GetTexture()
{
	return Data.tex;
}


