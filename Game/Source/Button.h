/*#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "GuiElements.h"

#include "Point.h"
#include "SString.h"

class Button : public GuiElements
{
public:

	Button(uint32 id, SDL_Rect bounds);
	virtual ~Button();

	bool Update(Input* input, float dt);
	bool Draw(Render* render);

};

#endif // !__BUTTON_H__*/

