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
#include "j1Collisions.h"
#include "j1Scene.h"
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
	player.speed.x = config.child("speed").attribute("x").as_float();
	player.speed.y = config.child("speed").attribute("y").as_float();
	player.max_speed.x = config.child("max_speed").attribute("x").as_float();
	player.max_speed.y = config.child("max_speed").attribute("y").as_float();
	player.acceleration.x = config.child("acceleration").attribute("x").as_float();
	player.acceleration.y = config.child("acceleration").attribute("y").as_float();

	player.gravity = config.child("gravity").attribute("value").as_float();

	player.hitbox_width = config.child("hitbox").attribute("w").as_int();
	player.hitbox_height = config.child("hitbox").attribute("h").as_int();

	return true;

}

bool j1Player::Start()
{
	SummonPlayer();
	return true;
}

bool j1Player::PreUpdate()
{
	if (player.disabled) //if the player is disable, it stops updating the logic
	{
		return true;
	}

	player.grounded = false;

	if (!CheckAirborne()) //Setting grounded states
	{
		player.current_state = player_states::IDLE;

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			player.current_state = player_states::CROUCH;
			player.able_to_drop = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT
			|| App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT
			|| App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT
			|| App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			player.current_state = player_states::RUNNING;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) //Jump if not on godmode
		{
			player.current_state = player_states::JUMP;
			player.speed.y = 0;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) //If player has to drop from platform
	{
		player.able_to_drop = true;
	}
	else
	{
		player.able_to_drop = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) // Able/Disable GodMode
	{
		GodMode();

	}
	return true;
}

bool j1Player::Update(float dt)
{
	if (player.disabled) //if the player is disable, it stops updating the logic
	{
		return true;
	}

	if (!CheckAirborne() && player.grounded)
	{
		player.speed.y = 0;
	}

	HorizontalMovement();

	if (!player.god_mode)
	{
		//State machine
		switch (player.current_state)
		{
		case player_states::IDLE:

			//LOG("Current State: IDLE");
			player.animation = "idle";
			player.speed.x = 0;


			break;
		case player_states::RUNNING:

			//LOG("Current State: RUNNING");
			player.animation = "run";


			break;
		case player_states::CROUCH:

			//LOG("Current State: CROUCH");
			//player.animation = "crouch";

			break;
		case player_states::JUMP:

			//LOG("Current State: JUMP");
			player.speed.y -= player.acceleration.y;
			player.jumping = true;

			break;
		case player_states::FALL:

			//LOG("Current State: FALL");
			player.jumping = true;

			break;

		}

		if (player.grounded)
		{
			player.jumping = false;
			player.current_state = player_states::IDLE;
		}
		else
		{
			player.current_state = player_states::FALL;
		}

		//Jump logic
		if (player.jumping)
		{
			player.speed.y += player.gravity; // Speed.y is +gravity when not grounded

			if (player.speed.y >= player.max_speed.y) // Speed.y is capped an maxSpeed
			{
				player.speed.y = player.max_speed.y;
			}

			if (player.speed.y < 0) // If on jump is going up uses jump animation
			{
				//player.animation = "jump";
			}
			else // If on jump is going down uses fall animation
			{
				//player.animation = "fall";
			}
		}

		//Update position
		player.position.y += player.speed.y;
		player.position.x += player.speed.x;
	}
	else //GodMode Activated!
	{
		//player.animation = "godmode";

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			MoveUp();
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			MoveDown();
		}
		player.position.x += player.speed.x * 2;
	}

	//Draw player
	App->map->DrawAnimation(player.animation, "playerx96", player.flip);


	//Update player collider and position

	player.player_collider->SetPos(player.position.x + 30, player.position.y);

	SetCamera();

	return true;
}

bool j1Player::PostUpdate()
{
	if (player.disabled) //if the player is disable, it stops updating the logic
	{
		return true;
	}

	return true;
}

bool j1Player::CleanUp()
{

	return true;
}

//Setting up the player on start
bool j1Player::SummonPlayer()
{
	player.position = App->map->data.starting_position;

	player.player_hitbox = { (int)player.position.x, (int)player.position.y, player.hitbox_width, player.hitbox_height };

	player.player_collider = App->collisions->AddCollider(player.player_hitbox, object_type::PLAYER, this);

	//booleans
	player.moving_right = false;
	player.moving_left	= false;
	player.jumping		= false;
	player.grounded		= false;
	player.able_to_drop = false;

	player.disabled		= false;
	player.god_mode		= false;

	player.flip			= false;

	return true;
}

//Setting up the player on death
void j1Player::ResetPlayer()
{

	player.player_collider->to_delete = true;
	SummonPlayer();
}

//Collisions
void j1Player::OnCollision(Collider* A, Collider* B) {

	if (B->type == object_type::PLAYER) {
		Collider temp = *A;
		A = B;
		B = &temp;
	}
	if (A->type != object_type::PLAYER) {
		return;
	}

	if (!player.god_mode)
	{
		// ------------ Player Colliding with the ground ------------------
		if (A->type == object_type::PLAYER && B->type == object_type::GROUND) {

			//Colliding from above
			if (A->rect.y + A->rect.h - player.max_speed.y - 2 < B->rect.y
				&& A->rect.x < B->rect.x + B->rect.w
				&& A->rect.x + A->rect.w > B->rect.x)
			{
				if (player.speed.y > 0)
				{
					player.speed.y = 0;
				}

				player.position.y = B->rect.y - player.player_collider->rect.h + 1;
				player.grounded = true;
				player.jumping = false;
			}
			//Colliding from the sides
			else if (A->rect.y + (A->rect.h * 1.0f / 4.0f) < B->rect.y + B->rect.h
				&& A->rect.y + (A->rect.h * 3.0f / 4.0f) > B->rect.y)
			{
				if ((A->rect.x + A->rect.w) < (B->rect.x + B->rect.w / 4))
				{ //Player to the left 
					player.position.x = B->rect.x - A->rect.w;

				}
				else if (A->rect.x > (B->rect.x + B->rect.w * 3 / 4))
				{ //Player to the right
					player.position.x = B->rect.x + B->rect.w;
				}
			}
			//from below
			else if (A->rect.y < (B->rect.y + B->rect.h))
			{
				player.speed.y = player.max_speed.y / 2;
				player.position.y = B->rect.y + B->rect.h;
			}
		}

		if (!player.able_to_drop)
		{
			// ------------ Player Colliding with platforms ------------------
			if (A->type == object_type::PLAYER && B->type == object_type::PLATFORM) {

				//Colliding from above
				if (A->rect.y + A->rect.h - player.max_speed.y - 5 < B->rect.y
					&& A->rect.x < B->rect.x + B->rect.w
					&& A->rect.x + A->rect.w > B->rect.x)
				{
					if (player.speed.y > 0)
					{
						player.speed.y = 0;
					}

					player.position.y = B->rect.y - player.player_collider->rect.h + 1;
					player.grounded = true;
					player.jumping = false;
				}
			}

		}
		

		// ------------ Player Colliding with death limit ------------------
		if (A->type == object_type::PLAYER && B->type == object_type::DEATH) {

			//from above
			if (A->rect.y + A->rect.h - player.max_speed.y - 5 < B->rect.y
				&& A->rect.x < B->rect.x + B->rect.w
				&& A->rect.x + A->rect.w > B->rect.x)
			{
				if (player.speed.y > 0)
				{
					player.speed.y = 0;
				}

				ResetPlayer();
			}
		}
	}

	if (App->scene->level_completed)
	{
		// ------------ Player Colliding with the goal ------------------
		if (A->type == object_type::PLAYER && B->type == object_type::GOAL) {

			//Colliding from above
			if (A->rect.y + A->rect.h - player.max_speed.y - 2 < B->rect.y
				&& A->rect.x < B->rect.x + B->rect.w
				&& A->rect.x + A->rect.w > B->rect.x)
			{
				ResetPlayer();
			}
			//Colliding from the sides
			else if (A->rect.y + (A->rect.h * 1.0f / 4.0f) < B->rect.y + B->rect.h
				&& A->rect.y + (A->rect.h * 3.0f / 4.0f) > B->rect.y)
			{
				ResetPlayer();
			}
		}
	}
	
}

//Handles movement on the x-axis and sets the proper flip
void j1Player::HorizontalMovement()
{
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT /*|| App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT*/)
	{
		player.moving_right = true;
		MoveRight();
		player.flip = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT /*|| App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT*/)
	{
		player.moving_left = true;
		MoveLeft();
		player.flip = true;
	}
	else
	{
		player.speed.x = 0;
	}
}

//Movement basic functions
void j1Player::MoveRight() // Move Right the player at set speed
{
	player.speed.x += player.acceleration.x;

	if (player.speed.x > player.max_speed.x)
	{
		player.speed.x = player.max_speed.x;
	}

}

void j1Player::MoveLeft() // Move Left the player at speed
{
	player.speed.x -= player.acceleration.x;

	if (player.speed.x < -player.max_speed.x)
	{
		player.speed.x = -player.max_speed.x;
	}
}

void j1Player::MoveDown() // Move Right the player at set speed
{
	player.position.y += (player.max_speed.y / 2);
}

void j1Player::MoveUp() // Move Right the player at set speed
{
	player.position.y -= (player.max_speed.y / 2);
}

//Toggles god mode
void j1Player::GodMode()
{
	if (player.god_mode)
	{
		player.god_mode = false;
	}
	else
	{
		LOG("GodMode Activated!");
		player.god_mode = true;
	}
}

//Checks if the player is in the air
bool j1Player::CheckAirborne()
{
	if (player.current_state != player_states::JUMP && player.current_state != player_states::FALL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//Centering camera on the player
void j1Player::SetCamera()
{
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT ||
		App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT ||
		App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT ||
		App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		return;
	}
	else
	{
		x_axis = (-player.position.x) + (App->win->screen_surface->w / 2);
		y_axis = (-player.position.y) + (App->win->screen_surface->h * .6);


		//Checks camera x limits
		if (App->render->camera.x <= App->scene->camera_left_limit && App->render->camera.x >= (App->scene->camera_right_limit + App->win->screen_surface->w))
		{
			App->render->camera.x = (int)x_axis;
		}

		if (App->render->camera.x >= App->scene->camera_left_limit && player.position.x < (App->win->screen_surface->w / 2) + 1)
		{
			App->render->camera.x = App->scene->camera_left_limit - 1;
		}

		else if (player.position.x > (-(App->scene->camera_right_limit) - (App->win->screen_surface->w / 2)))
		{
			App->render->camera.x = App->scene->camera_right_limit + App->win->screen_surface->w + 2;
		}

		App->render->camera.y = (int)y_axis;
	}
}