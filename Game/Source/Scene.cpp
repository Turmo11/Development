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
//#include "Pickups.h"
#include "EntityEnemy.h"
#include "FadeToBlack.h"
#include "Projectile.h" //includes Pickups.h
#include "Pathfinding.h"

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

	//Lives
	pugi::xml_node lives = config.child("lives");

	livesRect.x = lives.child("livesRect").attribute("x").as_int();
	livesRect.y = lives.child("livesRect").attribute("y").as_int();
	livesRect.h = lives.child("livesRect").attribute("h").as_int();
	livesRect.w = lives.child("livesRect").attribute("w").as_int();

	livesRect2.x = lives.child("livesRect2").attribute("x").as_int();
	livesRect2.y = lives.child("livesRect2").attribute("y").as_int();
	livesRect2.h = lives.child("livesRect2").attribute("h").as_int();
	livesRect2.w = lives.child("livesRect2").attribute("w").as_int();

	livesRect3.x = lives.child("livesRect3").attribute("x").as_int();
	livesRect3.y = lives.child("livesRect3").attribute("y").as_int();
	livesRect3.h = lives.child("livesRect3").attribute("h").as_int();
	livesRect3.w = lives.child("livesRect3").attribute("w").as_int();

	maxLives = lives.child("maxLives").attribute("value").as_int();
	playerLives = maxLives;

	cdRect.x = config.child("cdRect").attribute("x").as_int();
	cdRect.y = config.child("cdRect").attribute("y").as_int();
	cdRect.h = config.child("cdRect").attribute("h").as_int();
	cdRect.w = config.child("cdRect").attribute("w").as_int();


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
	livesTex = app->tex->Load("Assets/Textures/UI/lives.png");
	cdTex = app->tex->Load("Assets/Textures/UI/cooldown.png");
	pathDebugTex = app->tex->Load("Assets/Textures/UI/debug_path.png");
	shieldCdTex = app->tex->Load("Assets/Textures/UI/shield.png");

	SetUp(currentLevel);

	app->render->camera.x = app->render->startingCamPos.x;
	app->render->camera.y = app->render->startingCamPos.y;

	levelCompleted = false;

	showUI = true;
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

	DrawPath();

	if (showUI)
	{
		ShowUi();
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void Scene::DrawPath()
{
	// Draw path of the closest entity
	const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
	if (app->map->debug) {
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			app->render->DrawTexture(pathDebugTex, pos.x + 16, pos.y + 16);
		}
	}
}

void Scene::CheckLevelProgress()
{
	ListItem<Pickup*>* pickupIterator = app->pickups->pickupList.start;

	while (pickupIterator != NULL)
	{
		if (!pickupIterator->data->collected && pickupIterator->data->type == PickupType::LETTER)
		{
			return;
		}
		pickupIterator = pickupIterator->next;
	}
	levelCompleted = true;
}

void Scene::ShowUi()
{
	app->render->DrawQuad({ -app->render->camera.x, -app->render->camera.y + app->render->camera.h - 100, 300, 100 }, 0, 0, 0, 180);
	switch (playerLives)
	{
	case 3:
		app->render->DrawTexture(livesTex, -app->render->camera.x + 15, -app->render->camera.y + app->render->camera.h - 75, &livesRect3, false, 1.0f, true);
		break;
	case 2:
		app->render->DrawTexture(livesTex, -app->render->camera.x + 15, -app->render->camera.y + app->render->camera.h - 75, &livesRect2, false, 1.0f, true);
		break;
	case 1:
		app->render->DrawTexture(livesTex, -app->render->camera.x + 15, -app->render->camera.y + app->render->camera.h - 75, &livesRect, false, 1.0f, true);
		break;
	}
	app->projectile->active = true;
	
	app->player->ShowShieldUI(true);

}


void Scene::RestartScene()
{
	if (!firstGame)
	{
		playerLives = app->scene->maxLives;
		showUI = true;

		SetUp(currentLevel);
	}
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
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) 
	{
		app->map->debug = !app->map->debug;
	}
		

	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		if (app->frameCap == CAP_AT_60)
		{
			app->frameCap = CAP_AT_30;
		}
		else if (app->frameCap == CAP_AT_30)
		{
			app->frameCap = CAP_AT_60;
		}
	}

	//Gather all letters
	if (app->input->GetKey(SDL_SCANCODE_KP_MULTIPLY) == KEY_DOWN)
	{
		app->pickups->DebugCollectAll();
	}

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->projectile->SpawnBeam(app->player->player.flip);
	}

	//Lives

	//+1
	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
	{
		if (playerLives < maxLives)
		{
			playerLives++;
		}
	}
	//-1
	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
	{
		if (playerLives > 0)
		{
			playerLives--;
		}
	}


	//Volume
	if (app->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN)
	{
		if (app->audio->mVolume < app->audio->maxVolume)
		{
			app->audio->mVolume += 0.1f;
			app->audio->fxVolume += 0.1f;
			app->audio->SetFxVolume(app->audio->fxVolume);
			app->audio->SetMusicVolume(app->audio->mVolume);
		}

	}
	if (app->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN)
	{
		if (app->audio->mVolume > 0.0f)
		{
			app->audio->mVolume -= 0.1f;
			app->audio->fxVolume -= 0.1f;
			app->audio->SetFxVolume(app->audio->fxVolume);
			app->audio->SetMusicVolume(app->audio->mVolume);
		}
	}
	if (app->audio->mVolume > app->audio->maxVolume)
	{
		app->audio->mVolume = app->audio->maxVolume;
	}
	if (app->audio->mVolume < 0.0f)
	{
		app->audio->mVolume = 0.0f;
	}
}

void Scene::SetUp(int level)
{
	app->pickups->CleanUp();
	//app->enemy->CleanUp();

	LOG("SetUp level = %d", level);

	switch (level)
	{
	case 0:

		currentLevel = 0;
		app->audio->PlayMusic("Assets/Audio/Music/tutorial.ogg", 0.0f);
		app->pickups->CreatePickup(PickupType::LETTER, "alpha", { 1152, 704 });
		app->pickups->CreatePickup(PickupType::LETTER, "chi", { 1792, 576 });
		app->pickups->CreatePickup(PickupType::LETTER, "rho", { 1408, 512 });
		app->pickups->CreatePickup(PickupType::LETTER, "eta", { 1792, 384 });
		break;

	case 1:

		app->map->Load("tutorial.tmx");

		currentLevel = 1;

		cameraRect.y = -450;
		//cameraRect.y = 5000;
		cameraRect.h = -3800;

		app->audio->PlayMusic("Assets/Audio/Music/tutorial.ogg", 0.0f);
		app->pickups->CreatePickup(PickupType::LETTER, "alpha", { 2128, 2448 });
		app->pickups->CreatePickup(PickupType::LETTER, "chi", { 528, 3024 });
		app->pickups->CreatePickup(PickupType::LETTER, "rho", { 2960, 784 });
		app->pickups->CreatePickup(PickupType::LETTER, "eta", { 656, 1936 });
		app->pickups->SetGoal({ 1552, 656 });
		//app->enemy->CreateEnemy(EnemyType::SOUL, { 925, 3475 });
		app->enemy->CreateEnemy(EnemyType::F_SOUL, { 900, 3300 });
		app->pickups->CreatePickup(PickupType::HEALTH, "heart", { 925, 3536 });
		//app->enemy->CreateEnemy(EnemyType::SOUL, { 900, 3536 });
		break;

	case 2:

		app->map->Load("athena.tmx");

		currentLevel = 2;


		cameraRect.y = 5000;
		//cameraRect.y = -450;
		//cameraRect.h = -3800;
		cameraRect.h = -6300;

		app->audio->PlayMusic("Assets/Audio/Music/athena.ogg", 0.0f);
		app->pickups->CreatePickup(PickupType::LETTER, "psi", { 256, 880 });
		app->pickups->CreatePickup(PickupType::LETTER, "omega", { 1248, 3320 });
		app->pickups->CreatePickup(PickupType::LETTER, "mu", { 2656, 1984 });
		app->pickups->CreatePickup(PickupType::LETTER, "iota", { 2320, 1920 });
		app->pickups->SetGoal({ 1486, 1040 });
		//app->pickups->SetGoal({ 1552, 656 });
		////app->enemy->CreateEnemy(EnemyType::SOUL, { 925, 3475 });
		//app->enemy->CreateEnemy(EnemyType::SOUL, { 976, 3536 });
		break;

	default:
		break;
	}
}
