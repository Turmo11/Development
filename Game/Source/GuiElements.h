/*#ifndef __GUIELEMENTS_H__
#define __GUIELEMENTS_H__

#include "Input.h"
#include "Render.h"
#include "TitleScene.h"

#include "Point.h"
#include "SString.h"
#include "External/SDL/include/SDL.h"

enum class GuiType
{
	BUTTON, 
	TOGGLE,
	CHECKBOX
};

enum class GuiState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED
};

class GuiElements
{
public:

	GuiElements(GuiType type) : type(type), state(GuiState::NORMAL){}
	virtual bool Update(Input* input, float dt);
	virtual bool Draw(Render* render);
	void SetTexture(SDL_Texture* texture);
	void BotifyObserver(); 

	GuiType type;
	GuiState state;
	SDL_Texture* tex;

	SDL_Rect sizeButton;

};

#endif // !__GUIELEMENTS_H__*/