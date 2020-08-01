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
	player.tex_test = App->tex->Load("Assets/textures/test.png");

	player.speed.x = config.child("speed").attribute("x").as_float();
	player.speed.y = config.child("speed").attribute("y").as_float();
	player.max_speed.x = config.child("maxSpeed").attribute("x").as_float();
	player.max_speed.y = config.child("maxSpeed").attribute("y").as_float();
	player.acceleration.x = config.child("acceleration").attribute("x").as_float();
	player.acceleration.y = config.child("acceleration").attribute("y").as_float();

	player.gravity = config.child("gravity").attribute("value").as_float();

	player.hitbox_width = config.child("hibox").attribute("w").as_int();
	player.hitbox_height = config.child("hitbox").attribute("h").as_int();

	return true;

	/*bool ret = true;

	pos.x = config.child("position").attribute("x").as_int();
	pos.y = config.child("position").attribute("y").as_int();

	speed = config.child("speed").attribute("value").as_int();

	return ret;*/
}

bool j1Player::Start()
{
	SummonPlayer();

	

	//p_texture = App->tex->Load("textures/test1.jpeg");

	//srand(time(NULL));
	//is_dead = false;
	//score = 0;
	//current_state = player_states::IDLE;

	//hitbox = { pos.x, pos.y, 28, 55 };

	////gravity = 2;

	return true;
}

bool j1Player::PreUpdate()
{
	if (player.disabled) //if the player is disable, it stops updating the logic
	{
		return true;
	}

	player.grounded = false;

	if (!CheckAirborne() && player.grounded)
	{
		player.current_state = player_states::IDLE;

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			player.current_state = player_states::CROUCH;
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

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) // Able/Disable GodMode
	{
		GodMode();
	}

	return true;

	/*if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		is_dead = false;
		LOG("REVIVE");
	}*/

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
	}

	return true;*/
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
		switch (player.current_state)
		{
		case player_states::IDLE:

			player.animation = "idle";
			player.speed.x = 0;

			break;
		case player_states::RUNNING:

			player.animation = "run";

			break;
		case player_states::CROUCH:

			player.animation = "crouch";

			break;
		case player_states::JUMP:

			player.speed.y -= player.acceleration.y;
			player.jumping = true;

			break;
		case player_states::FALL:

			player.jumping = true;

			break;

		}

		if (!CheckAirborne() && player.grounded)
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

		player.position.y += player.speed.y; //Update position y
		player.position.x += player.speed.x;
	}
	else //GodMode Activated!
	{
		player.animation = "idle";

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

	//Update player collider and position
	player.player_hitbox.x = player.position.x;
	player.player_hitbox.y = player.position.y;

	player.player_collider->SetPos(player.position.x + 20, player.position.y); //Magic Numbers


	player.rect_test = { (int)player.position.x, (int)player.position.y, 538, 361 };
	App->render->Blit(player.tex_test, player.position.x, player.position.y, &player.rect_test, 1.0f);

	return true;

	//hitbox.x = pos.x;
	//hitbox.y = pos.y;

	//MovePlayer(dt);

	//App->render->DrawQuad(hitbox, 255, 255, 0, 255);
	////App->render->Blit(p_texture, );

	//return true;
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

	/*App->tex->UnLoad(p_texture);*/

	return true;
}

//void j1Player::MovePlayer(float dt)
//{
//
//	if (!is_dead && camera_pos)
//	{
//		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
//		{
//			pos.y -= speed;
//			hitbox.x = pos.x; hitbox.y = pos.y;
//			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
//		}
//		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
//		{
//			pos.y += speed;
//			hitbox.x = pos.x; hitbox.y = pos.y;
//			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
//		}
//		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
//		{
//			pos.x -= speed;
//			hitbox.x = pos.x; hitbox.y = pos.y;
//
//			if (App->render->camera.x < 0 && App->render->camera.x + pos.x >= 300)
//			{
//				App->render->camera.x = -(pos.x - 500);
//			}
//
//			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
//		}
//		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
//		{
//			pos.x += speed;
//			hitbox.x = pos.x; hitbox.y = pos.y;
//
//			if (App->render->camera.x <= 0 && App->render->camera.x + pos.x > 500)
//			{
//				App->render->camera.x = -(pos.x - 500);
//			}
//
//			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
//		}
//		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && jumping == false)
//		{
//			p_pos_y = pos.y;
//			jumping = true;
//			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
//		}
//	}
//
//	else
//	{
//		current_state = player_states::IDLE;
//	}
//
//	if (jumping)
//	{
//		if (jumpF > -2)
//		{
//			pos.y += jumpF;
//			jumpF += 0.05f;
//			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
//		}
//
//		else if (jumpF < 2)
//		{
//			pos.y += jumpF;
//			jumpF += 0.05f;
//			std::this_thread::sleep_for(std::chrono::milliseconds(speed));
//		}
//
//		if (pos.y == p_pos_y)
//		{
//			jumpF = -2.5f;
//			jumping = false;
//			LOG("%f", jumpF);
//		}
//
//	}
//
//}
//
//p2Point<int> j1Player::getPos() const
//{
//	return pos;
//}

bool j1Player::SummonPlayer()
{
	player.position = App->map->data.starting_position;

	player.player_hitbox = { (int)player.position.x, (int)player.position.y, player.hitbox_width, player.hitbox_height };

	player.player_collider = App->collisions->AddCollider(player.player_hitbox, object_type::PLAYER, this);

	//booleans
	player.moving_right = false;
	player.moving_left = false;
	player.jumping = false;
	player.grounded = false;

	player.disabled = false;
	player.god_mode = false;

	player.flip = false;

	return true;
}

void j1Player::OnCollision(Collider* A, Collider* B) {

	if (B->type == object_type::PLAYER) {
		Collider temp = *A;
		A = B;
		B = &temp;
	}
	if (A->type != object_type::PLAYER) {
		return;
	}

	// ------------ Player Colliding against solids ------------------
	if (A->type == object_type::PLAYER && B->type == object_type::GROUND) {

		//from above
		if (player.position.y + A->rect.h - player.max_speed.y - 5 < B->rect.y
			&& A->rect.x < B->rect.x + B->rect.w
			&& A->rect.x + A->rect.w > B->rect.x) 
		{
			if (player.speed.y > 0)
			{
				player.speed.y = 0;
			}

			player.position.y = B->rect.y - player.player_collider->rect.h + 1;
			player.grounded = true;
		}
	}
}

void j1Player::GodMode()
{
	if (player.god_mode)
	{
		player.god_mode = false;
	}
	else
	{
		player.god_mode = true;
	}
}

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

void j1Player::HorizontalMovement()
{
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		player.moving_right = true;
		MoveRight();
		player.flip = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
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
	player.position.y += player.max_speed.y;
}

void j1Player::MoveUp() // Move Right the player at set speed
{
	player.position.y -= player.max_speed.y;
}