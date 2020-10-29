#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Collisions.h"
#include "Pickups.h"
#include "Scene.h"
#include "WalkingEnemy.h"
#include "Audio.h"


Pickups::Pickups() : Module()
{
	name.create("pickups");
}


// Destructor
Pickups::~Pickups()
{}

bool Pickups::Start()
{


	return true;
}

bool Pickups::PreUpdate()
{
	return true;
}

bool Pickups::Update(float dt)
{
	DrawAnimations();
	UpdateGoal();

	return true;
}

bool Pickups::PostUpdate()
{
	return true;
}

bool Pickups::CleanUp()
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

void Pickups::CreatePickup(p2SString name, iPoint position)
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

void Pickups::DrawAnimations()
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

void Pickups::OnCollision(Collider* A, Collider* B)
{
	if (A->type == object_type::LETTER && B->type == object_type::PLAYER)
	{
		A->to_delete = true;
		GetCollected();
		App->scene->CheckLevelProgress();

	}
}

void Pickups::GetCollected()
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

void Pickups::DebugCollectAll()
{
	p2List_item<Pickup*>* pickup_iterator = pickup_list.start;

	while (pickup_iterator != NULL)
	{
		pickup_iterator->data->collected = true;
		pickup_iterator = pickup_iterator->next;
	}

	App->scene->CheckLevelProgress();
}


void Pickups::SetGoal(iPoint position)
{
	Goal* left_goal = new Goal;
	Goal* right_goal = new Goal;

	left_goal->position = position;

	right_goal->position.x = position.x + 192;
	right_goal->position.y = position.y;

	goal_list.add(left_goal);
	goal_list.add(right_goal);
}

void Pickups::UpdateGoal()
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


