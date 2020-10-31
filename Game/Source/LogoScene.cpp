#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "LogoScene.h"
#include "EntityPlayer.h"
#include "FadeToBlack.h"
#include "Scene.h"

LogoScene::LogoScene() : Module()
{
	name.Create("LogoScene");
}

// Destructor
LogoScene::~LogoScene()
{}

// Called before render is available
bool LogoScene::Awake(pugi::xml_node& config)
{
	logoRect.x = config.child("logoRect").attribute("x").as_int();
	logoRect.y = config.child("logoRect").attribute("y").as_int();
	logoRect.h = config.child("logoRect").attribute("h").as_int();
	logoRect.w = config.child("logoRect").attribute("w").as_int();

	duration = config.child("duration").attribute("value").as_float();

	LOG("Loading LogoScene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool LogoScene::Start()
{
	app->fadeToBlack->activeScene = "LogoScene";

	accumulatedTime = 0.0f;

	logoTexture = app->tex->Load("Assets/textures/UI/uwu_logo.png");

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	return true;
}

// Called each loop iteration
bool LogoScene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool LogoScene::Update(float dt)
{
	DebugKeys();
	app->render->camera.x = 0;
	app->render->camera.y = 0;

	SDL_SetTextureAlphaMod(logoTexture, accumulatedTime * 10.0f);
	app->render->DrawTexture(logoTexture, 275, 50, &logoRect, false);

	SString title("Metamorphosis - Camera:(%d,%d)", app->render->camera.x, app->render->camera.y);

	app->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool LogoScene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	AutomaticTransition();

	return ret;
}

// Called before quitting
bool LogoScene::CleanUp()
{
	LOG("Freeing LogoScene");

	return true;
}

void LogoScene::DebugKeys()
{
	//Debug keys
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->fadeToBlack->FadeToBlackScene("TitleScene");

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

void LogoScene::AutomaticTransition()
{
	accumulatedTime += 0.075f;

	if (accumulatedTime >= duration)
	{
		app->fadeToBlack->FadeToBlackScene("TitleScene");
	}
}

