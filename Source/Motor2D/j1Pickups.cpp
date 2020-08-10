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
#include "j1Pickups.h"
#include <math.h>
#include <thread>         
#include <chrono>   
#include <time.h>


j1Pickups::j1Pickups() : j1Module()
{
	name.create("pickups");
}


// Destructor
j1Pickups::~j1Pickups()
{}

bool j1Pickups::Start()
{


	// Un collider q vaig posar x veure si anava 
	pickup.pickup_hitbox.x = 1000;
	pickup.pickup_hitbox.y = 720;
	pickup.pickup_hitbox.w = 32;
	pickup.pickup_hitbox.h = 32;

	pickup.pickup_collider = App->collisions->AddCollider(pickup.pickup_hitbox, object_type::LETTER, this);

	return true;
}

bool j1Pickups::PreUpdate()
{
	return true;
}

bool j1Pickups::Update(float dt)
{
	return true;
}

bool j1Pickups::PostUpdate()
{
	return true;
}

bool j1Pickups::CleanUp()
{
	return true;
}

void j1Pickups::OnCollision(Collider* A, Collider* B)
{
	if (A->type == object_type::LETTER && B->type == object_type::PLAYER)
	{
		A->to_delete = true;
	}
}
