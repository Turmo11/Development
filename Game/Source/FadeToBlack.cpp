#include <math.h>

#include "Application.h"
#include "Module.h"
#include "FadeToBlack.h"
#include "Window.h"
#include "Log.h"
#include "Render.h"
#include "EntityPlayer.h"
#include "Collisions.h"
#include "Pickups.h"
//#include "WalkingEnemy.h"
#include "Map.h"
#include "Scene.h"
#include "TitleScene.h"
#include "GameOverScene.h"
#include "LogoScene.h"
#include "External/SDL/include/SDL_render.h"
#include "External/SDL/include/SDL_timer.h"

FadeToBlack::FadeToBlack()
{
	name.Create("fade");

}

FadeToBlack::~FadeToBlack()
{}

bool FadeToBlack::Awake(pugi::xml_node& conf) {


	uint width, height;
	app->win->GetWindowSize(width, height);
	screen.w = width + 200;
	screen.h = height;
	screen.x = screen.y = 0;

	return true;
}

// Load assets
bool FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);

	fadingPlayer = false;
	sceneSwitch = false;
	wantToSwitchScene = "Scene";

	return true;
}

// Update: draw background
bool FadeToBlack::Update(float dt)
{

	if (currentStep == FadeStep::NONE)
		return true;

	Uint32 now = SDL_GetTicks() - startTime;
	float normalized = MIN(1.0f, (float)now / (float)totalTime);

	switch (currentStep)
	{
	case FadeStep::FADE_TO:
	{
		if (now >= totalTime)
		{
			if (!sceneSwitch)
			{

				if (fadingPlayer)
				{
					app->player->ResetPlayer();
					app->scene->showUI = true;
				}
				else
				{
					SwitchMap(nextLevel);
				}
			}
			else
			{
				SwitchScenes(wantToSwitchScene);
			}
			totalTime += totalTime;
			startTime = SDL_GetTicks();
			currentStep = FadeStep::FADE_FROM;
		}
	} break;

	case FadeStep::FADE_FROM:
	{
		normalized = 1.0f - normalized;

		if (now >= totalTime)
			currentStep = FadeStep::NONE;
		app->player->player.disabled = false;
		fadingPlayer = false;
		sceneSwitch = false;
	} break;
	}

	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(app->render->renderer, &screen);

	return true;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool FadeToBlack::DoFadeToBlack(int level, float time)
{
	bool ret = false;

	nextLevel = level;

	if (currentStep == FadeStep::NONE)
	{
		currentStep = FadeStep::FADE_TO;
		startTime = SDL_GetTicks();
		totalTime = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;

		app->player->player.disabled = true;
	}

	return ret;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool FadeToBlack::FadeToBlackPlayerOnly(float time)
{
	bool ret = false;

	fadingPlayer = true;

	if (currentStep == FadeStep::NONE)
	{
		currentStep = FadeStep::FADE_TO;
		startTime = SDL_GetTicks();
		totalTime = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;

		app->player->player.disabled = true;
	}

	return ret;
}

bool FadeToBlack::SwitchMap(int level) 
{
	bool ret = true;
	LOG("Switching Maps...");

	app->collisions->colliders.clear(); //Clear colliders

	// Remove all tilesets
	ListItem<TileSet*>* item;
	item = app->map->data.tilesets.start;

	while (item != NULL)
	{
		item->data->animations.clear();
		delete item->data->playerTileRect;


		SDL_DestroyTexture(item->data->texture);

		RELEASE(item->data);
		item = item->next;
	}

	app->map->data.tilesets.clear();

	// Remove all layers
	ListItem<MapLayer*>* item2;
	item2 = app->map->data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}

	app->map->data.layers.clear();

	//Object cleanup
	ListItem<ObjectGroup*>* item3;
	item3 = app->map->data.objectGroups.start;

	while (item3 != NULL)
	{
		if (item3->data->objects != NULL)
			delete[] item3->data->objects;

		delete item3->data;

		item3 = item3->next;
	}

	app->map->data.objectGroups.clear();

	app->scene->SetUp(level);		//Load specified map
	app->collisions->LoadFromMap();		//Load Collisions
	app->player->ResetPlayer();	//Reset Player

	return ret;
}

bool FadeToBlack::FadeToBlackScene(char* scene, float time)
{
	bool ret = false;

	sceneSwitch = true;
	wantToSwitchScene = scene;

	if (currentStep == FadeStep::NONE)
	{
		currentStep = FadeStep::FADE_TO;
		startTime = SDL_GetTicks();
		totalTime = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	return ret;
}

bool FadeToBlack::SwitchScenes(char* scene)
{
	if (activeScene != scene)
	{
		if (scene == "TitleScene")
		{
			app->scene->CleanUp();
			app->scene->active = false;
			app->titleScene->active = true;
			app->gameOverScene->active = false;
			app->logoScene->active = false;

			app->player->active = false;
			app->pickups->active = false;
			//app->walkingEnemy->active = false;
			app->map->active = false;
		}
		if (scene == "GameOverScene")
		{
			app->scene->CleanUp();
			app->scene->active = false;
			app->titleScene->active = false;
			app->gameOverScene->active = true;
			app->logoScene->active = false;

			app->player->active = false;
			app->pickups->active = false;
			//app->walkingEnemy->active = false;
			app->map->active = false;
		}
		if (scene == "LogoScene")
		{
			app->scene->CleanUp();
			app->scene->active = false;
			app->titleScene->active = false;
			app->gameOverScene->active = false;
			app->logoScene->active = true;

			app->player->active = false;
			app->pickups->active = false;
			//app->walkingEnemy->active = false;
			app->map->active = false;
		}
		if (scene == "Scene")
		{
			app->scene->active = true;
			app->titleScene->active = false;
			app->gameOverScene->active = false;
			app->logoScene->active = false;

			app->player->active = true;
			app->pickups->active = true;
			//app->walkingEnemy->active = true;
			app->map->active = true;
		}

		activeScene = scene;
	}

	return true;
}