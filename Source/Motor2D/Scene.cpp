#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Scene.h"
#include "E_PLayer.h"
#include "Pickups.h"
#include "WalkingEnemy.h"
#include "FadeToBlack.h"

Scene::Scene() : Module()
{
	name.create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	currentLevel = config.child("currentLevel").attribute("value").as_int();

	backgroundRect.x = config.child("backgroundRect").attribute("x").as_int();
	backgroundRect.y = config.child("backgroundRect").attribute("y").as_int();
	backgroundRect.h = config.child("backgroundRect").attribute("h").as_int();
	backgroundRect.w = config.child("backgroundRect").attribute("w").as_int();

	cameraRect.x = config.child("cameraRect").attribute("x").as_int();
	cameraRect.y = config.child("cameraRect").attribute("y").as_int();
	cameraRect.h = config.child("cameraRect").attribute("h").as_int();
	cameraRect.w = config.child("cameraRect").attribute("w").as_int();

	levelCompleted = config.child("levelCompleted").attribute("value").as_bool();

	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	App->fade_to_black->active_scene = "Scene";

	backgroundRect.h = 6816;
	backgroundRect.w = 3072;
	backgroundRect.x = 0;
	backgroundRect.y = 0;

	background = App->tex->Load("Assets/textures/tower3.png");

	currentLevel = 1;
	SetUp(currentLevel);
	
	App->render->camera.x = App->render->starting_cam_pos.x;
	App->render->camera.y = App->render->starting_cam_pos.y;

	levelCompleted = false;
	
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	DebugKeys();	
	
	switch (currentLevel)
	{
	case 1:
		App->render->Blit(background, -800, -5500, &backgroundRect, false, 0.1f);
		break;
	case 2:
		App->render->Blit(background, -800, -4700, &backgroundRect, false, 0.1f);
		break;
	}
	
	App->map->Draw();

	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Camera:(%d,%d) Player:(%.2f,%.2f)",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(), App->render->camera.x, App->render->camera.y,
					App->player->player.position.x, App->player->player.position.y);

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	

	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;


}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void Scene::CheckLevelProgress()
{
	p2List_item<Pickup*>* pickup_iterator = App->pickups->pickup_list.start;

	while (pickup_iterator != NULL)
	{
		if (!pickup_iterator->data->collected)
		{
			return;
		}
		pickup_iterator = pickup_iterator->next;
	}
	levelCompleted = true;
}

void Scene::DebugKeys()
{
	//Debug keys
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->fade_to_black->DoFadeToBlack(1);
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->fade_to_black->DoFadeToBlack(2);
	}
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->fade_to_black->DoFadeToBlack(currentLevel);
	}
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		App->win->ToggleFullscreen();
	}
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->SaveGame();
	}
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		App->LoadGame();
	}
	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		App->fade_to_black->FadeToBlackScene("TitleScene");
	}
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		App->fade_to_black->FadeToBlackScene("GameOverScene");
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_MULTIPLY) == KEY_DOWN)
	{
		App->pickups->DebugCollectAll();
	}

	//Volume
	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN)
	{
		if (App->audio->volume < App->audio->max_volume)
		{
			App->audio->volume += 0.1f;
			App->audio->SetFxVolume(App->audio->volume);
			App->audio->SetMusicVolume(App->audio->volume);
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN)
	{
		if (App->audio->volume > 0.0f)
		{
			App->audio->volume -= 0.1f;
			App->audio->SetFxVolume(App->audio->volume);
			App->audio->SetMusicVolume(App->audio->volume);
		}
	}
	if (App->audio->volume > App->audio->max_volume)
	{
		App->audio->volume = App->audio->max_volume;
	}
	if (App->audio->volume < 0.0f)
	{
		App->audio->volume = 0.0f;
	}
}

void Scene::SetUp(int level)
{
	App->pickups->CleanUp();
	App->walking_enemy->CleanUp();

	LOG("SetUp level = %d", level);

	switch (level)
	{
	case 0:

		currentLevel = 0;
		App->audio->PlayMusic("Assets/audio/music/tutorial.ogg", 0.0f);
		App->pickups->CreatePickup("alpha", { 1152, 704 });
		App->pickups->CreatePickup("chi", { 1792, 576 });
		App->pickups->CreatePickup("rho", { 1408, 512 });
		App->pickups->CreatePickup("eta", { 1792, 384 });
		break;

	case 1:

		App->map->Load("tutorial.tmx");

		currentLevel = 1;

		cameraRect.y = -450;
		cameraRect.h = -3800;

		App->audio->PlayMusic("Assets/audio/music/tutorial.ogg", 0.0f);
		App->pickups->CreatePickup("alpha", { 2128, 2448 });
		App->pickups->CreatePickup("chi", { 528, 3024 });
		App->pickups->CreatePickup("rho", { 2960, 784 });
		App->pickups->CreatePickup("eta", { 656, 1936 });
		App->pickups->SetGoal({ 1552, 656 });
		//App->walking_enemy->CreateEnemy(enemy_type::SOUL, { 925, 3475 });
		//App->walking_enemy->CreateEnemy(enemy_type::SOUL, { 976, 3536 });
		break;

	case 2:

		App->map->Load("athena.tmx");

		currentLevel = 2;

		cameraRect.y = 5000;
		cameraRect.h = -6300;

		App->audio->PlayMusic("Assets/audio/music/athena.ogg", 0.0f);
		App->pickups->CreatePickup("psi", { 1583, 2736 });
		//App->pickups->CreatePickup("chi", { 528, 3024 });
		//App->pickups->CreatePickup("rho", { 2960, 784 });
		//App->pickups->CreatePickup("eta", { 656, 1936 });
		//App->pickups->SetGoal({ 1552, 656 });
		////App->walking_enemy->CreateEnemy(enemy_type::SOUL, { 925, 3475 });
		//App->walking_enemy->CreateEnemy(enemy_type::SOUL, { 976, 3536 });
		break;

	default:
		break;
	}
}
