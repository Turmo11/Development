#ifndef __LogoScene_H__
#define __LogoScene_H__

#include "Module.h"

struct SDL_Texture;

class LogoScene : public Module
{
public:

	LogoScene();

	// Destructor
	virtual ~LogoScene();

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

public:

	void DebugKeys();
	void AutomaticTransition();

public:

	float	volume;
	float	max_volume;

private:

	SDL_Rect		logo_rect;
	SDL_Texture*	logo_texture;

	float	accumulatedTime;
	float	duration;
};

#endif // __SCENE_H__