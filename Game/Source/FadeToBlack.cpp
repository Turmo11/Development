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
	App->win->GetWindowSize(width, height);
	screen.w = width + 200;
	screen.h = height;
	screen.x = screen.y = 0;

	return true;
}

// Load assets
bool FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

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
					App->player->ResetPlayer();
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
		App->player->player.disabled = false;
		fadingPlayer = false;
		sceneSwitch = false;
	} break;
	}

	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

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

		App->player->player.disabled = true;
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

		App->player->player.disabled = true;
	}

	return ret;
}

bool FadeToBlack::SwitchMap(int level) 
{
	bool ret = true;
	LOG("Switching Maps...");

	App->collisions->colliders.clear(); //Clear colliders

	// Remove all tilesets
	List_item<TileSet*>* item;
	item = App->map->data.tilesets.start;

	while (item != NULL)
	{
		item->data->animations.clear();
		delete item->data->playerTileRect;


		SDL_DestroyTexture(item->data->texture);

		RELEASE(item->data);
		item = item->next;
	}

	App->map->data.tilesets.clear();

	// Remove all layers
	List_item<MapLayer*>* item2;
	item2 = App->map->data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}

	App->map->data.layers.clear();

	//Object cleanup
	List_item<ObjectGroup*>* item3;
	item3 = App->map->data.objectGroups.start;

	while (item3 != NULL)
	{
		if (item3->data->objects != NULL)
			delete[] item3->data->objects;

		delete item3->data;

		item3 = item3->next;
	}

	App->map->data.objectGroups.clear();

	App->scene->SetUp(level);		//Load specified map
	App->collisions->LoadFromMap();		//Load Collisions
	App->player->ResetPlayer();	//Reset Player

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
			App->scene->CleanUp();
			App->scene->active = false;
			App->titleScene->active = true;
			App->gameOverScene->active = false;
			App->logo_scene->active = false;

			App->player->active = false;
			App->pickups->active = false;
			//App->walkingEnemy->active = false;
			App->map->active = false;
		}
		if (scene == "GameOverScene")
		{
			App->scene->CleanUp();
			App->scene->active = false;
			App->titleScene->active = false;
			App->gameOverScene->active = true;
			App->logo_scene->active = false;

			App->player->active = false;
			App->pickups->active = false;
			//App->walkingEnemy->active = false;
			App->map->active = false;
		}
		if (scene == "LogoScene")
		{
			App->scene->CleanUp();
			App->scene->active = false;
			App->titleScene->active = false;
			App->gameOverScene->active = false;
			App->logo_scene->active = true;

			App->player->active = false;
			App->pickups->active = false;
			//App->walkingEnemy->active = false;
			App->map->active = false;
		}
		if (scene == "Scene")
		{
			App->scene->active = true;
			App->titleScene->active = false;
			App->gameOverScene->active = false;
			App->logo_scene->active = false;

			App->player->active = true;
			App->pickups->active = true;
			//App->walkingEnemy->active = true;
			App->map->active = true;
		}

		activeScene = scene;
	}

	return true;
}