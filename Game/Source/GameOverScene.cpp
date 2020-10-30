#include "p2Defs.h"
#include "p2Log.h"
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
	name.create("GameOverScene");
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
	App->fadeToBlack->activeScene = "GameOverScene";

	background = App->tex->Load("Assets/textures/UI/gameover.png");

	backgroundRect.h = 512;
	backgroundRect.w = 1024;
	backgroundRect.x = 0;
	backgroundRect.y = 0;

	App->render->camera.x = 0;
	App->render->camera.y = 0;

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
	App->audio->StopMusic();
	DebugKeys();
	App->render->camera.x = 0;
	App->render->camera.y = 0;

	App->render->Blit(background, 100, 125, &backgroundRect, false);

	p2SString title("Metamorphosis - Camera:(%d,%d)", App->render->camera.x, App->render->camera.y);

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool GameOverScene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
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
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		App->fadeToBlack->FadeToBlackScene("TitleScene");
	
	}
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->fadeToBlack->DoFadeToBlack(1);
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->fadeToBlack->DoFadeToBlack(2);
	}
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		App->win->ToggleFullscreen();
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


