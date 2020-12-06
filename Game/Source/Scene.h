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

	void CheckLevelProgress();
	void SetUp(int level);
	void DebugKeys();

	void LoadCheckpoint();

	//UI
	void ShowUi();
	void RestartScene();

	void DrawPath();

public:

	int				currentLevel;

	Point<float> checkpointPos;

	bool			levelCompleted; //used to check if the player has collected all the pickups

	SDL_Rect		cameraRect;

	int playerLives;
	int maxLives;

	bool showUI;
	bool firstGame = true;

	SDL_Texture* shieldCdTex;

private:

	SDL_Rect		backgroundRect;
	SDL_Texture*	background;

	//UI
	SDL_Rect livesRect;
	SDL_Rect livesRect2;
	SDL_Rect livesRect3;
	SDL_Texture* livesTex;

	SDL_Rect cdRect;
	SDL_Texture* cdTex;

	SDL_Rect pathDebugRect;
	SDL_Texture* pathDebugTex;

	
};

#endif // __SCENE_H__