/*#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "GuiElements.h"
#include "Button.h"
#include "List.h"

class GuiManager : public Module
{
public:
	GuiManager();
	virtual ~GuiManager();

	bool Awake(pugi::xml_node&);

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	GuiElements* CreateElement(GuiType type);

	List<GuiElements*> elements;

	float accTime = 0.0f;
	float updateMs = 0.0f;
	bool doLogic = false;

	SDL_Rect button = { 0, 0, 120, 120 };

};




#endif*/