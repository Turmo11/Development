#ifndef __GAME_OVER_SCENE_H__
#define __GAME_OVER_SCENE_H__

#include "Module.h"

struct SDL_Texture;

class GameOverScene : public Module
{
public:

	GameOverScene();

	// Destructor
	virtual ~GameOverScene();

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


private:

	SDL_Rect	backgroundRect;
	SDL_Texture* background;

};

#endif // __SCENE_H__