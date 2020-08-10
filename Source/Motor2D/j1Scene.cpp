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
	App->map->Load("map.tmx");
	App->render->camera.x = App->render->starting_cam_pos.x;
	App->render->camera.y = App->render->starting_cam_pos.y;

	camera_left_limit = 0;
	camera_right_limit = -2500;

	alpha_pos = { 1152, 704 };
	chi_pos = { 1792, 576 };
	rho_pos = { 1408, 512 };
	eta_pos = { 1792, 384 };
	
	//test_work_pls = { (int)App->player->player.position.x, App->player->player.position.y, 200, 200 };
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
	/*if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame();

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame();*/

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 2;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 2;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 2;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 2;

	App->map->Draw();
	App->map->DrawStaticAnimation("alpha_shine", "letter_tileset", alpha_pos);
	/*App->map->DrawStaticAnimation("chi_shine", "letter_tileset", chi_pos);
	App->map->DrawStaticAnimation("rho_shine", "letter_tileset", rho_pos);
	App->map->DrawStaticAnimation("eta_shine", "letter_tileset", eta_pos);*/

	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Camera:(%d,%d) Player:(%.2f,%.2f)",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(), App->render->camera.x, -App->render->camera.y,
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
