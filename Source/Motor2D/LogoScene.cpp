#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "LogoScene.h"
#include "E_PLayer.h"
#include "FadeToBlack.h"
#include "Scene.h"

LogoScene::LogoScene() : Module()
{
	name.create("LogoScene");
}

// Destructor
LogoScene::~LogoScene()
{}

// Called before render is available
bool LogoScene::Awake()
{
	LOG("Loading LogoScene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool LogoScene::Start()
{
	App->fade_to_black->active_scene = "LogoScene";

	accumulatedTime = 0.f;
	duration = 20.0f;

	volume = 0.1f;
	max_volume = 1.0f;

	App->audio->SetFxVolume(volume);
	App->audio->SetMusicVolume(volume);

	logo_texture = App->tex->Load("Assets/textures/uwu_logo.png");

	logo_rect.h = 640;
	logo_rect.w = 640;
	logo_rect.x = 0;
	logo_rect.y = 0;

	App->render->camera.x = 0;
	App->render->camera.y = 0;

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
	App->render->camera.x = 0;
	App->render->camera.y = 0;

	SDL_SetTextureAlphaMod(logo_texture, accumulatedTime * 10.0f);
	App->render->Blit(logo_texture, 300, 50, &logo_rect, false);

	p2SString title("Metamorphosis - Camera:(%d,%d)", App->render->camera.x, App->render->camera.y);

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool LogoScene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
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
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		App->fade_to_black->FadeToBlackScene("TitleScene");
	
	}
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		App->win->ToggleFullscreen();
	}

	//Volume
	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN)
	{
		if (volume < max_volume)
		{
			volume += 0.1f;
			App->audio->SetFxVolume(volume);
			App->audio->SetMusicVolume(volume);
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN)
	{
		if (volume > 0.0f)
		{
			volume -= 0.1f;
			App->audio->SetFxVolume(volume);
			App->audio->SetMusicVolume(volume);
		}
	}
	if (volume > max_volume)
	{
		volume = max_volume;
	}
	if (volume < 0.0f)
	{
		volume = 0.0f;
	}
}

void LogoScene::AutomaticTransition()
{
	accumulatedTime += 0.075f;

	if (accumulatedTime >= duration)
	{
		App->fade_to_black->FadeToBlackScene("TitleScene");

	}
}

