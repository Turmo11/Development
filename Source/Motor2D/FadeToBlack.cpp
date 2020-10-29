#include <math.h>

#include "Application.h"
#include "Module.h"
#include "FadeToBlack.h"
#include "Window.h"
#include "p2Log.h"
#include "Render.h"
#include "E_PLayer.h"
#include "Collisions.h"
#include "Pickups.h"
#include "WalkingEnemy.h"
#include "Map.h"
#include "Scene.h"
#include "TitleScene.h"
#include "GameOverScene.h"
#include "Dependencies/SDL/include/SDL_render.h"
#include "Dependencies/SDL/include/SDL_timer.h"

FadeToBlack::FadeToBlack()
{
	name.create("fade");

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

	fading_player = false;
	scene_switch = false;
	wantToSwitchScene = "Scene";

	return true;
}

// Update: draw background
bool FadeToBlack::Update(float dt)
{

	if (current_step == fade_step::NONE)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::FADE_TO:
	{
		if (now >= total_time)
		{
			if (!scene_switch)
			{


				if (fading_player)
				{
					App->player->ResetPlayer();
				}
				else
				{
					SwitchMap(next_level);
				}
			}
			else
			{
				SwitchScenes(wantToSwitchScene);
			}
			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::FADE_FROM;
		}
	} break;

	case fade_step::FADE_FROM:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			current_step = fade_step::NONE;
		App->player->player.disabled = false;
		fading_player = false;
		scene_switch = false;
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

	next_level = level;

	if (current_step == fade_step::NONE)
	{
		current_step = fade_step::FADE_TO;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;

		App->player->player.disabled = true;
	}

	return ret;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool FadeToBlack::FadeToBlackPlayerOnly(float time)
{
	bool ret = false;

	fading_player = true;

	if (current_step == fade_step::NONE)
	{
		current_step = fade_step::FADE_TO;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;

		App->player->player.disabled = true;
	}

	return ret;
}

bool FadeToBlack::SwitchMap(int level) {
	bool ret = true;
	LOG("Switching Maps...");

	App->collisions->colliders.clear(); //Clear colliders

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = App->map->data.tilesets.start;

	while (item != NULL)
	{
		item->data->animations.clear();
		delete item->data->player_tile_rect;


		SDL_DestroyTexture(item->data->texture);

		RELEASE(item->data);
		item = item->next;
	}
	App->map->data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = App->map->data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	App->map->data.layers.clear();

	//Object cleanup
	p2List_item<ObjectGroup*>* item3;
	item3 = App->map->data.object_groups.start;

	while (item3 != NULL)
	{
		if (item3->data->objects != NULL)
			delete[] item3->data->objects;

		delete item3->data;

		item3 = item3->next;
	}

	App->map->data.object_groups.clear();

	App->scene->SetUp(level);		//Load specified map
	App->collisions->LoadFromMap();		//Load Collisions
	App->player->ResetPlayer();	//Reset Player

	return ret;
}

bool FadeToBlack::FadeToBlackScene(char* scene, float time)
{
	bool ret = false;

	scene_switch = true;
	wantToSwitchScene = scene;

	if (current_step == fade_step::NONE)
	{
		current_step = fade_step::FADE_TO;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;

	}

	return ret;
}

bool FadeToBlack::SwitchScenes(char* scene)
{
	if(scene == "TitleScene")
	{
		App->scene->active = false;
		App->game_over_scene->active = false;
		App->player->active = false;
		App->pickups->active = false;
		App->walking_enemy->active = false;
		App->map->active = false;
		App->title_scene->active = true;
	}
	if (scene == "GameOverScene")
	{
		App->game_over_scene->active = true;
		App->scene->active = false;
		App->title_scene->active = false;
		App->player->active = false;
		App->pickups->active = false;
		App->walking_enemy->active = false;
		App->map->active = false;
	}
	if (scene == "Scene")
	{
		App->scene->active = true;
		App->player->active = true;
		App->pickups->active = true;
		App->walking_enemy->active = true;
		App->map->active = true;
		App->title_scene->active = false;
		App->game_over_scene->active = false;
	}

	active_scene = scene;
	return true;
}