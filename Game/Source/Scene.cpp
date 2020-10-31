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
	App->fadeToBlack->activeScene = "Scene";

	background = App->tex->Load("Assets/textures/tower.png");

	SetUp(currentLevel);
	
	App->render->camera.x = App->render->startingCamPos.x;
	App->render->camera.y = App->render->startingCamPos.y;

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
		App->render->DrawTexture(background, -800, -5500, &backgroundRect, false, 0.1f);
		break;
	case 2:
		App->render->DrawTexture(background, -800, -4700, &backgroundRect, false, 0.1f);
		break;
	}
	
	App->map->Draw();

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Camera:(%d,%d) Player:(%.2f,%.2f)",
					App->map->data.width, App->map->data.height,
					App->map->data.tileWidth, App->map->data.tileHeight,
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
	List_item<Pickup*>* pickupIterator = App->pickups->pickupList.start;

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
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->fadeToBlack->DoFadeToBlack(1);
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->fadeToBlack->DoFadeToBlack(2);
	}
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->fadeToBlack->DoFadeToBlack(currentLevel);
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
		App->fadeToBlack->FadeToBlackScene("TitleScene");
	}
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		App->fadeToBlack->FadeToBlackScene("GameOverScene");
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_MULTIPLY) == KEY_DOWN)
	{
		App->pickups->DebugCollectAll();
	}

	//Volume
	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN)
	{
		if (App->audio->volume < App->audio->maxVolume)
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
	if (App->audio->volume > App->audio->maxVolume)
	{
		App->audio->volume = App->audio->maxVolume;
	}
	if (App->audio->volume < 0.0f)
	{
		App->audio->volume = 0.0f;
	}
}

void Scene::SetUp(int level)
{
	App->pickups->CleanUp();
	//App->walkingEnemy->CleanUp();

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
		//cameraRect.y = 5000;
		cameraRect.h = -3800;

		App->audio->PlayMusic("Assets/audio/music/tutorial.ogg", 0.0f);
		App->pickups->CreatePickup("alpha", { 2128, 2448 });
		App->pickups->CreatePickup("chi", { 528, 3024 });
		App->pickups->CreatePickup("rho", { 2960, 784 });
		App->pickups->CreatePickup("eta", { 656, 1936 });
		App->pickups->SetGoal({ 1552, 656 });
		//App->walkingEnemy->CreateEnemy(EnemyType::SOUL, { 925, 3475 });
		//App->walkingEnemy->CreateEnemy(EnemyType::SOUL, { 976, 3536 });
		break;

	case 2:

		App->map->Load("athena.tmx");

		currentLevel = 2;

	
		cameraRect.y = 5000;
		//cameraRect.y = -450;
		//cameraRect.h = -3800;
		cameraRect.h = -6300;

		App->audio->PlayMusic("Assets/audio/music/athena.ogg", 0.0f);
		App->pickups->CreatePickup("psi", { 1583, 2736 });
		//App->pickups->CreatePickup("chi", { 528, 3024 });
		//App->pickups->CreatePickup("rho", { 2960, 784 });
		//App->pickups->CreatePickup("eta", { 656, 1936 });
		//App->pickups->SetGoal({ 1552, 656 });
		////App->walkingEnemy->CreateEnemy(EnemyType::SOUL, { 925, 3475 });
		//App->walkingEnemy->CreateEnemy(EnemyType::SOUL, { 976, 3536 });
		break;

	default:
		break;
	}
}
