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

	enum class FadeStep
	{
		NONE,
		FADE_TO,
		FADE_FROM
		
	} currentStep = FadeStep::NONE;

public:

	int nextLevel;
	bool fadingPlayer;
	bool sceneSwitch;
	char* wantToSwitchScene;
	char* activeScene;

private:
	
	Uint32 startTime = 0;
	Uint32 totalTime = 0;
	SDL_Rect screen;

};

#endif //__MODULEFADETOBLACK_H__