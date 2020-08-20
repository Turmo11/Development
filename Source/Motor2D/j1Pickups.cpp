#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Collisions.h"
#include "j1Pickups.h"
#include "j1Scene.h"
#include "j1WalkingEnemy.h"
#include "j1Audio.h"


j1Pickups::j1Pickups() : j1Module()
{
	name.create("pickups");
}


// Destructor
j1Pickups::~j1Pickups()
{}

bool j1Pickups::Start()
{


	return true;
}

bool j1Pickups::PreUpdate()
{
	return true;
}

bool j1Pickups::Update(float dt)
{
	DrawAnimations();
	UpdateGoal();

	return true;
}

bool j1Pickups::PostUpdate()
{
	return true;
}

bool j1Pickups::CleanUp()
{
	// Remove pickups
	p2List_item<Pickup*>* item;
	item = pickup_list.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	pickup_list.clear();

	// Remove goals
	p2List_item<Goal*>* item2;
	item2 = goal_list.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	goal_list.clear();


	return true;
}

void j1Pickups::CreatePickup(p2SString name, iPoint position)
{
	Pickup* new_pickup = new Pickup;

	new_pickup->name = name;
	new_pickup->position = position;

	new_pickup->pickup_hitbox.x = position.x;
	new_pickup->pickup_hitbox.y = position.y;
	new_pickup->pickup_hitbox.w = 32;
	new_pickup->pickup_hitbox.h = 32;

	new_pickup->pickup_collider = App->collisions->AddCollider(new_pickup->pickup_hitbox, object_type::LETTER, this);

	new_pickup->collected = false;

	pickup_list.add(new_pickup);

}

void j1Pickups::SetUp(int level)
{
	switch (level)
	{
	case 0:

		App->audio->PlayMusic("Assets/audio/music/tutorial.ogg", 0.0f);
		CreatePickup("alpha", { 1152, 704 });
		CreatePickup("chi", { 1792, 576 });
		CreatePickup("rho", { 1408, 512 });
		CreatePickup("eta", { 1792, 384 });
		break;

	case 1:

		App->audio->PlayMusic("Assets/audio/music/athena.ogg", 0.0f);
		CreatePickup("alpha", { 2128, 2448 });
		CreatePickup("chi", { 528, 3024 });
		CreatePickup("rho", { 2960, 784 });
		CreatePickup("eta", { 656, 1936 });
		SetGoal({ 1552, 656 });
		App->walking_enemy->CreateEnemy(enemy_type::SOUL, { 925, 3475 });
		break;

	case 2:
		App->audio->PlayMusic("Assets/audio/music/athena.ogg", 0.0f);
		CreatePickup("alpha", { 2128, 2448 });
		CreatePickup("chi", { 528, 3024 });
		CreatePickup("rho", { 2960, 784 });
		CreatePickup("eta", { 656, 1936 });
		App->walking_enemy->CreateEnemy(enemy_type::SOUL, { 925, 3475 });
		
		break;
	}
}

void j1Pickups::DrawAnimations()
{
	p2List_item<Pickup*>* pickup_iterator = pickup_list.start;

	while (pickup_iterator != NULL)
	{
		if (!pickup_iterator->data->collected)
		{
			App->map->DrawStaticAnimation(pickup_iterator->data->name.GetString(), "letter_tileset", pickup_iterator->data->position, &pickup_iterator->data->anim_info);

		}
		pickup_iterator = pickup_iterator->next;
	}
}

void j1Pickups::OnCollision(Collider* A, Collider* B)
{
	if (A->type == object_type::LETTER && B->type == object_type::PLAYER)
	{
		A->to_delete = true;
		GetCollected();
		App->scene->CheckLevelProgress();

	}
}

void j1Pickups::GetCollected()
{
	p2List_item<Pickup*>* pickup_iterator = pickup_list.start;

	while (pickup_iterator != NULL)
	{
		if (pickup_iterator->data->pickup_collider->to_delete)
		{
			pickup_iterator->data->collected = true;
			LOG("Collected %s", pickup_iterator->data->name.GetString());
		}
		pickup_iterator = pickup_iterator->next;
	}
}

void j1Pickups::SetGoal(iPoint position)
{
	Goal* left_goal = new Goal;
	Goal* right_goal = new Goal;

	left_goal->position = position;

	right_goal->position.x = position.x + 192;
	right_goal->position.y = position.y;

	goal_list.add(left_goal);
	goal_list.add(right_goal);
}

void j1Pickups::UpdateGoal()
{
	p2List_item<Goal*>* goal_iterator = goal_list.start;

	while (goal_iterator != NULL)
	{
		if (App->scene->level_completed)
		{
			App->map->DrawStaticAnimation("goal", "goal", goal_iterator->data->position, &goal_iterator->data->anim_info);

		}
		goal_iterator = goal_iterator->next;
	}
}