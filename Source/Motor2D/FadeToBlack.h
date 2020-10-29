#ifndef __FADETOBLACK_H__
#define __FADETOBLACK_H__

#include "Module.h"
#include "Dependencies/SDL\include\SDL_rect.h"

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
	bool SwitchMap(int level);

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

private:
	
	Uint32		start_time = 0;
	Uint32		total_time = 0;
	SDL_Rect	screen;
	
	

};

#endif //__MODULEFADETOBLACK_H__