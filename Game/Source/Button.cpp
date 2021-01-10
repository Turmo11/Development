/*#include "Button.h"

Button::Button(uint32 id, SDL_Rect size) : GuiElements(GuiType::BUTTON)
{
	this->sizeButton = size;

}

Button::~Button()
{

}

bool Button::Update(Input* input, float dt)
{
	if (state != GuiState::DISABLED)
	{
		int mousePosX, mousePosY;
		input->GetMousePosition(mousePosX, mousePosY);

		if((mousePosX > sizeButton.x) && (mousePosX < (sizeButton.x + sizeButton.w)) && (mousePosY > sizeButton.y) && (mousePosY < (sizeButton.y + sizeButton.h)))
		{
			state = GuiState::FOCUSED;

			if (input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
			{
				state = GuiState::PRESSED;
			}

			if (input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
			{
				
			}
		}
	}

	return true;
}*/