#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "TitleScene.h"
#include "E_PLayer.h"
#include "FadeToBlack.h"

TitleScene::TitleScene() : Module()
{
	name.create("TitleScene");
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
	App->fade_to_black->active_scene = "TitleScene";

	volume = 0.1f;
	max_volume = 1.0f;

	App->audio->SetFxVolume(volume);
	App->audio->SetMusicVolume(volume);

	background = App->tex->Load("Assets/textures/title.png");


	App->render->camera.x = App->render->starting_cam_pos.x;
	App->render->camera.y = App->render->starting_cam_pos.y;

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

	
	App->render->Blit(background, -800, -5500, &background_rect, false, 0.1f);

	p2SString title("Metamorphosis");

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool TitleScene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
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
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		App->fade_to_black->FadeToBlackScene("Scene");
	
	}
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->fade_to_black->DoFadeToBlack(1);
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->fade_to_black->DoFadeToBlack(2);
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


