#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1Pickups.h"
#include "j1WalkingEnemy.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	volume = 1.0f;
	max_volume = 1.0f;

	App->audio->SetFxVolume(volume);
	App->audio->SetMusicVolume(volume);

	SetUp(2);
	
	App->render->camera.x = App->render->starting_cam_pos.x;
	App->render->camera.y = App->render->starting_cam_pos.y;

	level_completed = false;
	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	//Debug keys
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		SetUp(1);
		App->player->ResetPlayer();
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		SetUp(2);
		App->player->ResetPlayer();
	}
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->player->ResetPlayer();
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
bool j1Scene::PostUpdate()
{
	

	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	LOG("%f", volume);

	return ret;


}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void j1Scene::CheckLevelProgress()
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
	level_completed = true;
}

void j1Scene::SetUp(int level)
{
	switch (level)
	{
	case 0:

		App->audio->PlayMusic("Assets/audio/music/tutorial.ogg", 0.0f);
		App->pickups->CreatePickup("alpha", { 1152, 704 });
		App->pickups->CreatePickup("chi", { 1792, 576 });
		App->pickups->CreatePickup("rho", { 1408, 512 });
		App->pickups->CreatePickup("eta", { 1792, 384 });
		break;

	case 1:

		App->map->Load("tutorial.tmx");

		camera_left_limit = 0;
		camera_right_limit = -3150;
		camera_top_limit = -450;
		camera_bot_limit = -3800;

		App->audio->PlayMusic("Assets/audio/music/athena.ogg", 0.0f);
		App->pickups->CreatePickup("alpha", { 2128, 2448 });
		App->pickups->CreatePickup("chi", { 528, 3024 });
		App->pickups->CreatePickup("rho", { 2960, 784 });
		App->pickups->CreatePickup("eta", { 656, 1936 });
		App->pickups->SetGoal({ 1552, 656 });
		//App->walking_enemy->CreateEnemy(enemy_type::SOUL, { 925, 3475 });
		App->walking_enemy->CreateEnemy(enemy_type::SOUL, { 976, 3536 });
		break;

	case 2:

		App->map->Load("athena.tmx");

		camera_left_limit = 0;
		camera_right_limit = -3150;
		camera_top_limit = -450;
		camera_bot_limit = -3800;

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
