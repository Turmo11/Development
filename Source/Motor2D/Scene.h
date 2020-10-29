#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

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

	void CheckLevelProgress();
	void SetUp(int level);
	void DebugKeys();

public:

	int		camera_left_limit;
	int		camera_right_limit;
	int		camera_bot_limit;
	int		camera_top_limit;

	bool	level_completed; //used to check if the player has collected all the pickups

	float	volume;
	float	max_volume;

	int		current_level;
	

private:

	SDL_Rect		background_rect;
	SDL_Texture*	background;

};

#endif // __SCENE_H__