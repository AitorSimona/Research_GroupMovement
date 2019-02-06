#include "j1UI_Element.h"

j1UI_Element::j1UI_Element()
{
}

j1UI_Element::~j1UI_Element()
{
}

ELEMENTS j1UI_Element::GetType()
{
	return ELEMENTS::NONE;
}

Buttonrects * j1UI_Element::Getrects()
{
	return nullptr;
}

Booleans * j1UI_Element::GetBooleans()
{
	return nullptr;
}

SDL_Texture * j1UI_Element::GetTexture()
{
	return nullptr;
}

textbox * j1UI_Element::GetTexts()
{
	return nullptr;
}

