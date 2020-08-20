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
	App->map->Load("tutorial.tmx");
	
	

	App->render->camera.x = App->render->starting_cam_pos.x;
	App->render->camera.y = App->render->starting_cam_pos.y;

	camera_left_limit = 0;
	camera_right_limit = -3150;
	camera_top_limit = -450;
	camera_bot_limit = -3800;

	level_completed = false;
	App->pickups->SetUp(1);
	
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
