#ifndef __LOGO_SCENE_H__
#define __LOGO_SCENE_H__

#include "Module.h"

struct SDL_Texture;

class LogoScene : public Module
{
public:

	LogoScene();

	// Destructor
	virtual ~LogoScene();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

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

public:

	void DebugKeys();
	void AutomaticTransition();

private:

	SDL_Rect logoRect;
	SDL_Texture* logoTexture;

	float accumulatedTime;
	float duration;
};

#endif // __SCENE_H__