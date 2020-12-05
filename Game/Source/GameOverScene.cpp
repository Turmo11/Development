#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "GameOverScene.h"
#include "EntityPlayer.h"
#include "FadeToBlack.h"
#include "Scene.h"

GameOverScene::GameOverScene() : Module()
{
	name.Create("GameOverScene");
}

// Destructor
GameOverScene::~GameOverScene()
{}

// Called before render is available
bool GameOverScene::Awake()
{
	LOG("Loading GameOverScene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool GameOverScene::Start()
{
	app->fadeToBlack->activeScene = "GameOverScene";

	background = app->tex->Load("Assets/Textures/UI/gameover.png");

	backgroundRect.h = 512;
	backgroundRect.w = 1024;
	backgroundRect.x = 0;
	backgroundRect.y = 0;

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	return true;
}

// Called each loop iteration
bool GameOverScene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool GameOverScene::Update(float dt)
{
	app->audio->StopMusic();
	DebugKeys();
	app->render->camera.x = 0;
	app->render->camera.y = 0;

	app->render->DrawTexture(background, 120, 100, &backgroundRect, false);

	/*SString title("Metamorphosis - Camera:(%d,%d)", app->render->camera.x, app->render->camera.y);

	app->win->SetTitle(title.GetString());*/
	return true;
}

// Called each loop iteration
bool GameOverScene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool GameOverScene::CleanUp()
{
	LOG("Freeing GameOverScene");

	return true;
}

void GameOverScene::DebugKeys()
{
	//Debug keys
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->fadeToBlack->FadeToBlackScene("TitleScene");
	
	}
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		app->fadeToBlack->DoFadeToBlack(1);
	}
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		app->fadeToBlack->DoFadeToBlack(2);
	}
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		app->win->ToggleFullscreen();
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


