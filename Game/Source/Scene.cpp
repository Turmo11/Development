#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Scene.h"
#include "EntityPlayer.h"
#include "Pickups.h"
//#include "WalkingEnemy.h"
#include "FadeToBlack.h"

Scene::Scene() : Module()
{
	name.Create("scene");
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
	app->fadeToBlack->activeScene = "Scene";

	background = app->tex->Load("Assets/Textures/tower.png");

	SetUp(currentLevel);
	
	app->render->camera.x = app->render->startingCamPos.x;
	app->render->camera.y = app->render->startingCamPos.y;

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
		app->render->DrawTexture(background, -800, -5500, &backgroundRect, false, 0.1f);
		break;
	case 2:
		app->render->DrawTexture(background, -800, -4700, &backgroundRect, false, 0.1f);
		break;
	}
	
	app->map->Draw();

	/*SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Camera:(%d,%d) Player:(%.2f,%.2f)",
					app->map->data.width, app->map->data.height,
					app->map->data.tileWidth, app->map->data.tileHeight,
					app->map->data.tilesets.count(), app->render->camera.x, app->render->camera.y,
					app->player->player.position.x, app->player->player.position.y);

	app->win->SetTitle(title.GetString());*/
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
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
	ListItem<Pickup*>* pickupIterator = app->pickups->pickupList.start;

	while (pickupIterator != NULL)
	{
		if (!pickupIterator->data->collected)
		{
			return;
		}
		pickupIterator = pickupIterator->next;
	}
	levelCompleted = true;
}

void Scene::DebugKeys()
{
	//Debug keys
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		app->fadeToBlack->DoFadeToBlack(1);
	}
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		app->fadeToBlack->DoFadeToBlack(2);
	}
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		app->fadeToBlack->DoFadeToBlack(currentLevel);
	}
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		app->win->ToggleFullscreen();
	}
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		app->SaveGame();
	}
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		app->LoadGame();
	}
	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		app->fadeToBlack->FadeToBlackScene("TitleScene");
	}
	if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		app->fadeToBlack->FadeToBlackScene("GameOverScene");
	}
	if (app->input->GetKey(SDL_SCANCODE_KP_MULTIPLY) == KEY_DOWN)
	{
		app->pickups->DebugCollectAll();
	}

	//Volume
	if (app->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN)
	{
		if (app->audio->volume < app->audio->maxVolume)
		{
			app->audio->volume += 0.1f;
			app->audio->SetFxVolume(app->audio->volume);
			app->audio->SetMusicVolume(app->audio->volume);
		}

	}
	if (app->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN)
	{
		if (app->audio->volume > 0.0f)
		{
			app->audio->volume -= 0.1f;
			app->audio->SetFxVolume(app->audio->volume);
			app->audio->SetMusicVolume(app->audio->volume);
		}
	}
	if (app->audio->volume > app->audio->maxVolume)
	{
		app->audio->volume = app->audio->maxVolume;
	}
	if (app->audio->volume < 0.0f)
	{
		app->audio->volume = 0.0f;
	}
}

void Scene::SetUp(int level)
{
	app->pickups->CleanUp();
	//app->walkingEnemy->CleanUp();

	LOG("SetUp level = %d", level);

	switch (level)
	{
	case 0:

		currentLevel = 0;
		app->audio->PlayMusic("Assets/Audio/Music/tutorial.ogg", 0.0f);
		app->pickups->CreatePickup("alpha", { 1152, 704 });
		app->pickups->CreatePickup("chi", { 1792, 576 });
		app->pickups->CreatePickup("rho", { 1408, 512 });
		app->pickups->CreatePickup("eta", { 1792, 384 });
		break;

	case 1:

		app->map->Load("tutorial.tmx");

		currentLevel = 1;

		cameraRect.y = -450;
		//cameraRect.y = 5000;
		cameraRect.h = -3800;

		app->audio->PlayMusic("Assets/Audio/Music/tutorial.ogg", 0.0f);
		app->pickups->CreatePickup("alpha", { 2128, 2448 });
		app->pickups->CreatePickup("chi", { 528, 3024 });
		app->pickups->CreatePickup("rho", { 2960, 784 });
		app->pickups->CreatePickup("eta", { 656, 1936 });
		app->pickups->SetGoal({ 1552, 656 });
		//app->walkingEnemy->CreateEnemy(EnemyType::SOUL, { 925, 3475 });
		//app->walkingEnemy->CreateEnemy(EnemyType::SOUL, { 976, 3536 });
		break;

	case 2:

		app->map->Load("athena.tmx");

		currentLevel = 2;

	
		cameraRect.y = 5000;
		//cameraRect.y = -450;
		//cameraRect.h = -3800;
		cameraRect.h = -6300;

		app->audio->PlayMusic("Assets/Audio/Music/athena.ogg", 0.0f);
		app->pickups->CreatePickup("psi", { 1583, 2736 });
		//app->pickups->CreatePickup("chi", { 528, 3024 });
		//app->pickups->CreatePickup("rho", { 2960, 784 });
		//app->pickups->CreatePickup("eta", { 656, 1936 });
		//app->pickups->SetGoal({ 1552, 656 });
		////app->walkingEnemy->CreateEnemy(EnemyType::SOUL, { 925, 3475 });
		//app->walkingEnemy->CreateEnemy(EnemyType::SOUL, { 976, 3536 });
		break;

	default:
		break;
	}
}
