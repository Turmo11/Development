#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Render.h"
#include "Audio.h"
#include "Textures.h"
#include "E_Player.h"
#include "Window.h"
#include "Map.h"
#include "Input.h"
#include "Collisions.h"
#include "Scene.h"
#include "FadeToBlack.h"
#include <math.h>
#include <thread>         
#include <chrono>   
#include <time.h>


E_Player::E_Player() : Module()
{
	name.create("player");
}

// Destructor
E_Player::~E_Player()
{}

bool E_Player::Save(pugi::xml_node& node) const {

	LOG("Saving Player...");
	pugi::xml_node position = node.append_child("position");
	position.append_attribute("x") = player.position.x;
	position.append_attribute("y") = player.position.y;

	pugi::xml_node flags = node.append_child("flags");
	flags.append_attribute("jumping") = player.jumping;
	flags.append_attribute("able_to_drop") = player.able_to_drop;
	flags.append_attribute("grounded") = player.grounded;
	flags.append_attribute("flip") = player.flip;
	flags.append_attribute("god_mode") = player.god_mode;

	return true;
}

bool E_Player::Load(pugi::xml_node& node) {

	LOG("Loading Player...");

	player.disabled = true;
	player.speed.x = 0;
	player.speed.y = 0;

	pugi::xml_node position = node.child("position");

	player.position.x = position.attribute("x").as_float();
	player.position.y = position.attribute("y").as_float() - 1;

	pugi::xml_node flags = node.child("flags");
	player.able_to_drop = flags.attribute("drop_plat").as_bool();
	player.jumping = flags.attribute("jumping").as_bool();
	player.grounded = flags.attribute("playerGrounded").as_bool();
	player.god_mode = flags.attribute("godmode").as_bool();

	player.current_state = player_states::IDLE;
	player.disabled = false;

	return true;
}

bool E_Player::Awake(pugi::xml_node& config)
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

bool E_Player::Start()
{
	SummonPlayer();
	return true;
}

bool E_Player::PreUpdate()
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
		player.speed.x = 0;
		player.speed.y = 0;
		GodMode();

	}
	return true;
}

bool E_Player::Update(float dt)
{
	if (player.disabled) //if the player is disable, it stops updating the logic
	{
		return true;
	}

	if (!player.locked)
	{
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
					player.animation = "jump";
				}
				else // If on jump is going down uses fall animation
				{
					player.animation = "fall";
				}
			}

			if (player.colliding_wall)
			{
				player.speed.x = 0;
			}
		}
		else //GodMode Activated!
		{
			player.animation = "god";

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
	}


	//Update position
	player.position.y += player.speed.y;
	player.position.x += player.speed.x;

	//Draw player
	App->map->DrawAnimation(player.animation, "playerx96", player.flip);


	//Update player collider and position

	player.player_collider->SetPos(player.position.x + 30, player.position.y);

	player.colliding_wall = false;

	SetCamera();
	if (player.ascending)
	{
		Ascend();
	}

	return true;
}

bool E_Player::PostUpdate()
{
	if (player.disabled) //if the player is disable, it stops updating the logic
	{
		return true;
	}

	return true;
}

bool E_Player::CleanUp()
{

	return true;
}

//Setting up the player on start
bool E_Player::SummonPlayer()
{
	player.position = App->map->data.starting_position;

	player.player_hitbox = { (int)player.position.x, (int)player.position.y, player.hitbox_width, player.hitbox_height };

	player.player_collider = App->collisions->AddCollider(player.player_hitbox, object_type::PLAYER, this);

	//booleans
	player.moving_right = false;
	player.moving_left = false;
	player.jumping = false;
	player.grounded = false;
	player.colliding_wall = false;
	player.able_to_drop = false;

	player.disabled = false;
	player.locked = false;
	player.god_mode = false;
	player.ascending = false;

	player.flip = false;

	return true;
}

//Setting up the player on death
void E_Player::ResetPlayer()
{

	player.player_collider->to_delete = true;
	SummonPlayer();
}

//Collisions
void E_Player::OnCollision(Collider* A, Collider* B) {

	if (B->type == object_type::PLAYER) {
		Collider temp = *A;
		A = B;
		B = &temp;
	}
	if (A->type != object_type::PLAYER) {
		return;
	}

	if (!player.god_mode && !player.disabled)
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
				player.colliding_wall = true;
				LOG("Touching WALL");

				if ((A->rect.x + A->rect.w) < (B->rect.x + B->rect.w / 4))
				{ //Player to the left 
					player.position.x = B->rect.x - A->rect.w - 32;

				}
				else if (A->rect.x > (B->rect.x + B->rect.w * 3 / 4))
				{ //Player to the right
					player.position.x = B->rect.x + B->rect.w - 20;
				}

			}
			//from below
			else if (A->rect.y < (B->rect.y + B->rect.h))
			{
				player.speed.y = player.max_speed.y / 8;
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
				App->fade_to_black->FadeToBlackScene("GameOverScene");
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
				player.ascending = true;
			}
			//Colliding from the sides
			else if (A->rect.y + (A->rect.h * 1.0f / 4.0f) < B->rect.y + B->rect.h
				&& A->rect.y + (A->rect.h * 3.0f / 4.0f) > B->rect.y)
			{
				player.ascending = true;
			}
		}
	}

}

//Handles movement on the x-axis and sets the proper flip
void E_Player::HorizontalMovement()
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
void E_Player::MoveRight() // Move Right the player at set speed
{
	player.speed.x += player.acceleration.x;

	if (player.speed.x > player.max_speed.x)
	{
		player.speed.x = player.max_speed.x;
	}

}

void E_Player::MoveLeft() // Move Left the player at speed
{
	player.speed.x -= player.acceleration.x;

	if (player.speed.x < -player.max_speed.x)
	{
		player.speed.x = -player.max_speed.x;
	}
}

void E_Player::MoveDown() // Move Right the player at set speed
{
	player.position.y += (player.max_speed.y);
}

void E_Player::MoveUp() // Move Right the player at set speed
{
	player.position.y -= (player.max_speed.y);
}

//Toggles god mode
void E_Player::GodMode()
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
bool E_Player::CheckAirborne()
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
void E_Player::SetCamera()
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
		y_axis = (-player.position.y) + (App->win->screen_surface->h * .5);


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

		//Checks camera y limits
		if (App->render->camera.y <= App->scene->camera_top_limit && App->render->camera.y >= (App->scene->camera_bot_limit + App->win->screen_surface->h))
		{
			App->render->camera.y = (int)y_axis;
		}
		if (App->render->camera.y >= App->scene->camera_top_limit)
		{
			App->render->camera.y = App->scene->camera_top_limit;
		}
		else if (App->render->camera.y < (App->scene->camera_bot_limit + App->win->screen_surface->h))
		{
			App->render->camera.y = App->scene->camera_bot_limit + App->win->screen_surface->h;;
		}
	}
}

//Player completes level
void E_Player::Ascend(float time)
{

	player.position.y -= (player.max_speed.y / 8);

	switch (current_step)
	{
	case ascending::NONE:
	{
		current_step = ascending::ASCENDING;
		player.animation = "god";
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);


		player.locked = true;
		player.speed.x = 0;

		break;
	}

	case ascending::ASCENDING:
	{
		Uint32 now = SDL_GetTicks() - start_time;
		if (now >= total_time)
		{

			current_step = ascending::ASCENDED;
		}
		break;
	}
	case ascending::ASCENDED:
	{
		player.ascending = false;
		App->fade_to_black->DoFadeToBlack(App->scene->current_level + 1);
		current_step = ascending::NONE;
	}
	}








}