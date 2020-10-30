#ifndef __FADETOBLACK_H__
#define __FADETOBLACK_H__

#include "Module.h"
#include "External/SDL\include\SDL_rect.h"

class FadeToBlack : public Module
{
public:

	FadeToBlack();
	~FadeToBlack();

	bool Start();
	bool Awake(pugi::xml_node& conf);
	bool Update(float dt);
	bool DoFadeToBlack(int level, float time = 0.5f);
	bool FadeToBlackPlayerOnly(float time = 0.5f);
	bool FadeToBlackScene(char* scene, float time = 0.5f);
	bool SwitchMap(int level);
	bool SwitchScenes(char* scene);

private:

	enum class fade_step
	{
		NONE,
		FADE_TO,
		FADE_FROM
		
	} current_step = fade_step::NONE;

public:

	int			next_level;
	bool		fading_player;
	bool		scene_switch;
	char*		wantToSwitchScene;
	char*		active_scene;

private:
	
	Uint32		start_time = 0;
	Uint32		total_time = 0;
	SDL_Rect	screen;
	
	

};

#endif //__MODULEFADETOBLACK_H__