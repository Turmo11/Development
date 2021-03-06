#ifndef __TITLESCENE_H__
#define __TITLESCENE_H__

#include "Module.h"
#include "Button.h"

class GuiElements;

struct SDL_Texture;

class TitleScene : public Module
{
public:

	TitleScene();

	// Destructor
	virtual ~TitleScene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	virtual bool MouseClick(GuiElements* control)
	{
		return true;
	}

public:

	void DebugKeys();

private:

	SDL_Rect backgroundRect;
	SDL_Texture* background;

};

#endif // __SCENE_H__