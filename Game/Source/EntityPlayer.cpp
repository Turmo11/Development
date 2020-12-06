#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Render.h"
#include "Audio.h"
#include "Textures.h"
#include "EntityPlayer.h"
#include "EntityEnemy.h"
#include "Projectile.h"
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


EntityPlayer::EntityPlayer() : Module()
{
	name.Create("player");
}

// Destructor
EntityPlayer::~EntityPlayer()
{}

bool EntityPlayer::Save(pugi::xml_node& node) const
{
	LOG("Saving Player...");
	pugi::xml_node position = node.append_child("position");
	position.append_attribute("x") = player.position.x;
	position.append_attribute("y") = player.position.y - 10;

	pugi::xml_node flags = node.append_child("flags");
	flags.append_attribute("jumping") = player.jumping;
	flags.append_attribute("ableToDrop") = player.ableToDrop;
	flags.append_attribute("grounded") = player.grounded;
	flags.append_attribute("flip") = player.flip;
	flags.append_attribute("godMode") = player.godMode;

	return true;
}

bool EntityPlayer::Load(pugi::xml_node& node)
{
	LOG("Loading Player...");

	player.disabled = true;
	player.speed.x = 0;
	player.speed.y = 0;

	pugi::xml_node position = node.child("position");

	player.position.x = position.attribute("x").as_float();
	player.position.y = position.attribute("y").as_float();

	pugi::xml_node flags = node.child("flags");
	player.ableToDrop = flags.attribute("ableToDrop").as_bool();
	player.jumping = flags.attribute("jumping").as_bool();
	player.grounded = flags.attribute("grounded").as_bool();
	player.godMode = flags.attribute("godMode").as_bool();

	player.currentState = PlayerStates::IDLE;
	player.disabled = false;

	return true;
}

bool EntityPlayer::Awake(pugi::xml_node& config)
{
	player.speed.x = config.child("speed").attribute("x").as_float();
	player.speed.y = config.child("speed").attribute("y").as_float();
	player.maxSpeed.x = config.child("maxSpeed").attribute("x").as_float();
	player.maxSpeed.y = config.child("maxSpeed").attribute("y").as_float();
	player.acceleration.x = config.child("acceleration").attribute("x").as_float();
	player.acceleration.y = config.child("acceleration").attribute("y").as_float();

	player.gravity = config.child("gravity").attribute("value").as_float();

	player.hitboxWidth = config.child("hitbox").attribute("w").as_int();
	player.hitboxHeight = config.child("hitbox").attribute("h").as_int();

	projectileCooldown = config.child("projectile").attribute("cd").as_float();

	shieldCooldown = config.child("shield").attribute("cd").as_float();
	shieldDuration = config.child("shield").attribute("duration").as_float();
	checkpointCooldown = config.child("checkpoint").attribute("cd").as_float();


	return true;
}

bool EntityPlayer::Start()
{
	SummonPlayer();
	LoadSoundFx();

	shieldTex = app->tex->Load("Assets/Textures/Player/shield.png");
	showShieldUi = true;

	return true;
}

void EntityPlayer::LoadSoundFx()
{
	jumpSound = app->audio->LoadFx("Assets/Audio/Fx/jump.wav");
	beamSound = app->audio->LoadFx("Assets/Audio/Fx/beam.wav");
	healthSound = app->audio->LoadFx("Assets/Audio/Fx/health.wav");
	pickupSound = app->audio->LoadFx("Assets/Audio/Fx/pickup.wav");
	hitSound = app->audio->LoadFx("Assets/Audio/Fx/hit.wav");
	deathSound = app->audio->LoadFx("Assets/Audio/Fx/death.wav");
}


bool EntityPlayer::PreUpdate()
{
	if (player.disabled) //if the player is disable, it stops updating the logic
	{

		return true;
	}

	player.grounded = false;

	if (!CheckAirborne()) //Setting grounded states
	{
		player.currentState = PlayerStates::IDLE;

		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			player.currentState = PlayerStates::CROUCH;
			player.ableToDrop = true;
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			player.currentState = PlayerStates::RUNNING;
		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) //Jump if not on godmode
		{
			player.currentState = PlayerStates::JUMP;
			player.speed.y = 0;
			app->audio->PlayFx(jumpSound);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) //If player has to drop from platform
	{
		player.ableToDrop = true;
	}
	else
	{
		player.ableToDrop = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) // Able/Disable GodMode
	{
		player.speed.x = 0;
		player.speed.y = 0;
		GodMode();

	}


	return true;
}

bool EntityPlayer::Update(float dt)
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
		if (projectileTimer != 0.0f)
		{
			projectileTimer += dt;
			if (projectileTimer >= projectileCooldown)
			{
				projectileTimer = 0.0f;
				app->projectile->showCd = false;
			}
		}

		if (projectileTimer == 0.0f && app->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		{
			app->projectile->SpawnBeam(player.flip);
			projectileTimer += dt;
			app->projectile->showCd = true;
			app->audio->PlayFx(beamSound);
		}
		if (shieldTimer == 0.0f && app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		{
			player.shielded = true;
			shieldTimer += dt;

			//app->audio->PlayFx(beamSound);
		}

		if (shieldTimer != 0.0f)
		{
			if (player.shielded)
			{
				shieldTimer += dt;
				if (shieldDuration - shieldTimer < 2.0f && !blinkShield)
				{
					blinkShield = true;
					accumulatedTime = 0.0f;
				}
				if (shieldTimer >= shieldDuration)
				{
					shieldTimer = 0.1f;
					player.shielded = false;
					blinkShield = false;
					showShieldUi = false;
					showShieldCd = true;

				}
			}
			else
			{
				shieldTimer += dt;
				if (shieldTimer >= shieldCooldown)
				{
					shieldTimer = 0.0f;
					showShieldCd = false;
					showShieldUi = true;
				}
			}
		}


		// Cooldown to tp a checkpoint

		if (checkpointTimer != 0.0f)
		{
			checkpointTimer += dt;
			if (checkpointTimer >= checkpointCooldown)
			{
				checkpointTimer = 0.0f;
			}
		}

		if (checkpointTimer == 0.0f && app->input->GetKey(SDL_SCANCODE_KP_1) == KEY_DOWN)
		{
			app->scene->LoadCheckpoint();
			checkpointTimer += dt;
		}

		HorizontalMovement(dt);

		if (!player.godMode)
		{
			//State machine
			switch (player.currentState)
			{
			case PlayerStates::IDLE:

				//LOG("Current State: IDLE");
				player.animation = "idle";
				player.speed.x = 0;


				break;
			case PlayerStates::RUNNING:

				//LOG("Current State: RUNNING");
				player.animation = "run";


				break;
			case PlayerStates::CROUCH:

				//LOG("Current State: CROUCH");
				//player.animation = "crouch";

				break;
			case PlayerStates::JUMP:

				//LOG("Current State: JUMP");
				player.speed.y -= player.acceleration.y * app->GetDt();
				player.jumping = true;

				break;
			case PlayerStates::FALL:

				//LOG("Current State: FALL");
				player.jumping = true;

				break;

			}

			if (player.grounded)
			{
				player.jumping = false;
				player.currentState = PlayerStates::IDLE;
			}
			else
			{
				player.currentState = PlayerStates::FALL;
			}

			//Jump logic
			if (player.jumping)
			{
				player.speed.y += player.gravity * app->GetDt(); // Speed.y is +gravity when not grounded

				if (player.speed.y >= player.maxSpeed.y) // Speed.y is capped an maxSpeed
				{
					player.speed.y = player.maxSpeed.y;
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

			if (player.collidingWall)
			{
				player.speed.x = 0;
			}
		}
		else //GodMode Activated!
		{
			player.animation = "god";

			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				MoveUp(dt);
			}
			else if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				MoveDown(dt);
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				player.position.x -= (player.maxSpeed.x * dt);
			}
			else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				player.position.x += (player.maxSpeed.x * dt);
			}
			
		}
	}

	//Update position
	player.position.y += player.speed.y;
	player.position.x += player.speed.x;

	//Draw player
	app->map->DrawAnimation(player.animation, "playerx96", player.flip);

	//Update player collider and position
	player.playerCollider->SetPos(player.position.x + 30, player.position.y);

	player.collidingWall = false;

	SetCamera();
	if (player.ascending)
	{
		Ascend();
	}

	if (player.shielded)
	{
		if (!blinkShield)
		{
			app->render->DrawTexture(shieldTex, player.position.x, player.position.y + 2);
		}
		else
		{
			SDL_SetTextureAlphaMod(shieldTex, accumulatedTime * 100.0f);
			app->render->DrawTexture(shieldTex, player.position.x, player.position.y + 2);
			accumulatedTime += 7.5f * dt;
		}
	}



	return true;
}

void EntityPlayer::ShowShieldUI(bool active)
{
	if (app->fadeToBlack->activeScene == "Scene")
	{
		if (active)
		{
			if (showShieldUi)
			{
				app->render->DrawTexture(app->scene->shieldCdTex, -app->render->camera.x + 245, -app->render->camera.y + app->render->camera.h - 67);
			}
			else if(showShieldCd)
			{
				app->map->DrawStaticAnimation("yellow_cooldown", "yellow_cooldown", { -app->render->camera.x + 261, -app->render->camera.y + app->render->camera.h - 52 }, &app->projectile->animInfoYellow);
			}
		}
	}
}


bool EntityPlayer::PostUpdate()
{
	if (player.disabled) //if the player is disable, it stops updating the logic
	{
		return true;
	}

	if (addLife)
	{
		AddLife();
	}

	return true;
}

bool EntityPlayer::CleanUp()
{
	return true;
}

//Setting up the player on start
bool EntityPlayer::SummonPlayer()
{
	player.position = { app->scene->checkpointPos.x, app->scene->checkpointPos.y - app->player->player.hitboxHeight };
	//player.position.y = ;

	player.playerHitbox = { (int)player.position.x, (int)player.position.y, player.hitboxWidth, player.hitboxHeight };

	player.playerCollider = app->collisions->AddCollider(player.playerHitbox, ObjectType::PLAYER, this);

	//booleans
	player.movingRight = false;
	player.movingLeft = false;
	player.jumping = false;
	player.grounded = false;
	player.collidingWall = false;
	player.ableToDrop = false;

	player.disabled = false;
	player.locked = false;
	player.godMode = false;
	player.ascending = false;

	player.flip = false;

	return true;
}

//Setting up the player on death
void EntityPlayer::ResetPlayer()
{
	player.playerCollider->toDelete = true;
	SummonPlayer();
}

//Collisions
void EntityPlayer::OnCollision(Collider* A, Collider* B)
{
	if (B->type == ObjectType::PLAYER) {
		Collider temp = *A;
		A = B;
		B = &temp;
	}
	if (A->type != ObjectType::PLAYER) {
		return;
	}

	if (!player.godMode && !player.disabled)
	{
		// ------------ Player Colliding with the ground ------------------
		if (A->type == ObjectType::PLAYER && B->type == ObjectType::GROUND) {

			//Colliding from above
			if (A->rect.y + A->rect.h - (player.maxSpeed.y / 50) - 2 < B->rect.y
				&& A->rect.x < B->rect.x + B->rect.w
				&& A->rect.x + A->rect.w > B->rect.x)
			{
				if (player.speed.y > 0)
				{
					player.speed.y = 0;
				}

				player.position.y = B->rect.y - player.playerCollider->rect.h + 1;
				player.grounded = true;
				player.jumping = false;
			}
			//Colliding from the sides
			else if (A->rect.y + (A->rect.h * 1.0f / 4.0f) < B->rect.y + B->rect.h
				&& A->rect.y + (A->rect.h * 3.0f / 4.0f) > B->rect.y)
			{
				player.collidingWall = true;
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
				player.speed.y = player.maxSpeed.y / 8 * app->GetDt();
				player.position.y = B->rect.y + B->rect.h;
			}
		}

		if (!player.ableToDrop)
		{
			// ------------ Player Colliding with platforms ------------------
			if (A->type == ObjectType::PLAYER && B->type == ObjectType::PLATFORM) {

				//Colliding from above
				if (A->rect.y + A->rect.h < B->rect.y + (B->rect.h / 2.0f)
					&& A->rect.x < B->rect.x + B->rect.w
					&& A->rect.x + A->rect.w > B->rect.x && player.speed.y >= 0)
				{
					
					player.position.y = B->rect.y - player.playerCollider->rect.h + 2;
					player.grounded = true;
					player.jumping = false;
					if (player.speed.y > 0)
					{
						player.speed.y = 0;
					}
				}
			}

		}

		// ------------ Player Colliding with death limit ------------------
		if (A->type == ObjectType::PLAYER && B->type == ObjectType::DEATH) {

			//from above
			if (A->rect.y + A->rect.h - player.maxSpeed.y - 5 < B->rect.y
				&& A->rect.x < B->rect.x + B->rect.w
				&& A->rect.x + A->rect.w > B->rect.x)
			{
				if (player.speed.y > 0)
				{
					player.speed.y = 0;
				}

				TakeLife();
				return;
			}
		}
		if (!player.shielded)
		{
			// ------------ Player Colliding with enemy ------------------
			if (A->type == ObjectType::PLAYER && B->type == ObjectType::ENEMY)
			{
				TakeLife();
			}
		}

		// ------------ Player Colliding with checkpoint ------------------
		if (A->type == ObjectType::PLAYER && B->type == ObjectType::CHECKPOINT)
		{
			app->scene->checkpointPos.x = B->rect.x;
			app->scene->checkpointPos.y = B->rect.y;

			B->toDelete = true;
		}
	}

	if (app->scene->levelCompleted)
	{
		// ------------ Player Colliding with the goal ------------------
		if (A->type == ObjectType::PLAYER && B->type == ObjectType::GOAL) {

			//Colliding from above
			if (A->rect.y + A->rect.h - player.maxSpeed.y - 2 < B->rect.y
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
void EntityPlayer::HorizontalMovement(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT /*|| app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT*/)
	{
		player.movingRight = true;
		MoveRight(dt);
		player.flip = false;
	}
	else if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT /*|| app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT*/)
	{
		player.movingLeft = true;
		MoveLeft(dt);
		player.flip = true;
	}
	else
	{
		player.speed.x = 0;
	}
}

//Movement basic functions
void EntityPlayer::MoveRight(float dt) // Move Right the player at set speed
{
	player.speed.x += player.acceleration.x * dt;

	if (player.speed.x > player.maxSpeed.x * dt)
	{
		player.speed.x = player.maxSpeed.x * dt;
	}
}

void EntityPlayer::MoveLeft(float dt) // Move Left the player at speed
{
	player.speed.x -= player.acceleration.x * dt;

	if (player.speed.x < -player.maxSpeed.x * dt)
	{
		player.speed.x = -player.maxSpeed.x * dt;
	}
}

void EntityPlayer::MoveDown(float dt) // Move Right the player at set speed
{
	player.position.y += (player.maxSpeed.y * dt);
}

void EntityPlayer::MoveUp(float dt) // Move Right the player at set speed
{
	player.position.y -= (player.maxSpeed.y * dt);
}

//Toggles god mode
void EntityPlayer::GodMode()
{
	if (player.godMode)
	{
		player.godMode = false;
	}
	else
	{
		LOG("GodMode Activated!");
		player.godMode = true;
	}
}

//Checks if the player is in the air
bool EntityPlayer::CheckAirborne()
{
	if (player.currentState != PlayerStates::JUMP && player.currentState != PlayerStates::FALL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//Centering camera on the player
void EntityPlayer::SetCamera()
{
	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT ||
		app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT ||
		app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT ||
		app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		return;
	}
	else
	{
		xAxis = (-player.position.x) + (app->win->screenSurface->w / 2);
		yAxis = (-player.position.y) + (app->win->screenSurface->h * .5);

		//Checks camera x limits
		if (app->render->camera.x <= app->scene->cameraRect.x && app->render->camera.x >= (app->scene->cameraRect.w + app->win->screenSurface->w))
		{
			app->render->camera.x = (int)xAxis;
		}

		if (app->render->camera.x >= app->scene->cameraRect.x && player.position.x < (app->win->screenSurface->w / 2) + 1)
		{
			app->render->camera.x = app->scene->cameraRect.x - 1;
		}
		else if (player.position.x > (-(app->scene->cameraRect.w) - (app->win->screenSurface->w / 2)))
		{
			app->render->camera.x = app->scene->cameraRect.w + app->win->screenSurface->w + 2;
		}

		//Checks camera y limits
		if (app->render->camera.y <= app->scene->cameraRect.y && app->render->camera.y >= (app->scene->cameraRect.h + app->win->screenSurface->h))
		{
			app->render->camera.y = (int)yAxis;
		}

		if (app->render->camera.y >= app->scene->cameraRect.y)
		{
			app->render->camera.y = app->scene->cameraRect.y;
		}
		else if (app->render->camera.y < (app->scene->cameraRect.h + app->win->screenSurface->h))
		{
			app->render->camera.y = app->scene->cameraRect.h + app->win->screenSurface->h;;
		}
	}
}

void EntityPlayer::TakeLife()
{
	app->scene->showUI = false;
	app->audio->PlayFx(deathSound);
	if (app->scene->playerLives > 0)
	{
		app->fadeToBlack->FadeToBlackPlayerOnly(1.0f);
		app->scene->playerLives--;
		LOG("Lives: %d", app->scene->playerLives);
		int newScore;
		srand(time(NULL));
		newScore = (rand() % 400) + 100;
		if (app->scene->GetScore() - newScore > 0)
		{
			app->scene->AddScore(-newScore);
		}
		else
		{
			app->scene->SetScore(0);
		}
		
	}
	else
	{
		ResetPlayer();
		app->fadeToBlack->FadeToBlackScene("GameOverScene");
	}

}

void EntityPlayer::AddLife()
{
	if (app->scene->playerLives < app->scene->maxLives)
	{
		app->scene->playerLives++;
	}
	int newScore;
	srand(time(NULL));
	newScore = (rand() % 300) + 150;
	app->scene->AddScore(newScore);
	addLife = false;
}

//Player completes level
void EntityPlayer::Ascend(float time)
{
	player.position.y -= (player.maxSpeed.y / 8);

	switch (currentStep)
	{
	case Ascending::NONE:
	{
		currentStep = Ascending::ASCENDING;
		player.animation = "god";
		startTime = SDL_GetTicks();
		totalTime = (Uint32)(time * 0.5f * 1000.0f);


		player.locked = true;
		player.speed.x = 0;

		break;
	}

	case Ascending::ASCENDING:
	{
		Uint32 now = SDL_GetTicks() - startTime;
		if (now >= totalTime)
		{

			currentStep = Ascending::ASCENDED;
		}
		break;
	}
	case Ascending::ASCENDED:
	{
		player.ascending = false;
		app->fadeToBlack->DoFadeToBlack(app->scene->currentLevel + 1);
		currentStep = Ascending::NONE;
	}
	}
}
