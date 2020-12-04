#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "TitleScene.h"
#include "EntityPlayer.h"
#include "FadeToBlack.h"
#include "Scene.h"

TitleScene::TitleScene() : Module()
{
	name.Create("TitleScene");
}

// Destructor
TitleScene::~TitleScene()
{}

// Called before render is available
bool TitleScene::Awake()
{
	LOG("Loading TitleScene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool TitleScene::Start()
{
	app->fadeToBlack->activeScene = "TitleScene";

	background = app->tex->Load("Assets/textures/UI/title.png");

	backgroundRect.h = 512;
	backgroundRect.w = 1024;
	backgroundRect.x = 0;
	backgroundRect.y = 0;

	app->audio->StopMusic();

	app->render->camera.x = app->render->startingCamPos.x;
	app->render->camera.y = app->render->startingCamPos.y;

	return true;
}

// Called each loop iteration
bool TitleScene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool TitleScene::Update(float dt)
{
	DebugKeys();
	app->render->camera.x = 0;
	app->render->camera.y = 0;

	app->render->DrawTexture(background, 120, 100, &backgroundRect, false);

	/*SString title("Metamorphosis - Camera:(%d,%d)", app->render->camera.x, app->render->camera.y);

	app->win->SetTitle(title.GetString());*/
	return true;
}

// Called each loop iteration
bool TitleScene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool TitleScene::CleanUp()
{
	LOG("Freeing TitleScene");

	return true;
}

void TitleScene::DebugKeys()
{
	//Debug keys
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->fadeToBlack->FadeToBlackScene("Scene");
		app->audio->ResumeMusic();
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


