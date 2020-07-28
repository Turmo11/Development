#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Textures.h"
#include "j1Player.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Input.h"
#include <math.h>
#include <thread>         
#include <chrono>   
#include <time.h>


j1Player::j1Player() : j1Module()
{
	name.create("player");
}

// Destructor
j1Player::~j1Player()
{}

bool j1Player::Awake(pugi::xml_node& config)
{
	bool ret = true;

	pos.x = config.child("position").attribute("x").as_int();
	pos.y = config.child("position").attribute("y").as_int();

	speed = config.child("speed").attribute("value").as_int();

	return ret;
}

bool j1Player::Start()
{

	p_texture = App->tex->Load("textures/test1.jpeg");

	srand(time(NULL));
	is_dead = false;
	score = 0;
	c_state = STOP;

	hitbox = { pos.x, pos.y, 28, 55 };

	//gravity = 2;

	return true;
}

bool j1Player::PreUpdate()
{

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		is_dead = false;
		LOG("REVIVE");
	}

	/*if (!is_dead && camera_pos)
	{

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			c_state = LEFT;
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			c_state = RIGHT;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			c_state = JUMP;
		}
	}

	else
	{
		c_state = STOP;
	}*/

	return true;
}

bool j1Player::Update(float dt)
{
	
	hitbox.x = pos.x;
	hitbox.y = pos.y;

	MovePlayer(dt);

	App->render->DrawQuad(hitbox, 255, 255, 0, 255);
	//App->render->Blit(p_texture, );

	return true;
}

bool j1Player::PostUpdate()
{
	

	return true;
}

bool j1Player::CleanUp()
{

	App->tex->UnLoad(p_texture);

	return true;
}

void j1Player::MovePlayer(float dt)
{

	if (!is_dead && camera_pos)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			pos.y -= speed;
			hitbox.x = pos.x; hitbox.y = pos.y;
			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			pos.y += speed;
			hitbox.x = pos.x; hitbox.y = pos.y;
			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			pos.x -= speed;
			hitbox.x = pos.x; hitbox.y = pos.y;

			if (App->render->camera.x < 0 && App->render->camera.x + pos.x >= 300)
			{
				App->render->camera.x = -(pos.x - 500);
			}
			
			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			pos.x += speed;
			hitbox.x = pos.x; hitbox.y = pos.y;

			if (App->render->camera.x <= 0 && App->render->camera.x + pos.x > 500)
			{
				App->render->camera.x = -(pos.x - 500);
			}		

			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
		}
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && jumping == false)
		{
			p_pos_y = pos.y;
			jumping = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
		}
	}

	else
	{
		c_state = STOP;
	}

	if (jumping)
	{
		if (jumpF > -2)
		{
			pos.y += jumpF;
			jumpF += 0.05f;
			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
		}

		else if(jumpF < 2)
		{
			pos.y += jumpF;
			jumpF += 0.05f;
			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
		}

		if (pos.y == p_pos_y) 
		{
			jumpF = -2.5f;
			jumping = false;
			LOG("%f", jumpF);
		}

	}

}

p2Point<int> j1Player::getPos() const 
{
	return pos;
}