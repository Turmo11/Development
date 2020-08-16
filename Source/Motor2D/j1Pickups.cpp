#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Collisions.h"
#include "j1Pickups.h"
#include "j1Scene.h"



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

		CreatePickup("alpha", { 1152, 704 });
		CreatePickup("chi", { 1792, 576 });
		CreatePickup("rho", { 1408, 512 });
		CreatePickup("eta", { 1792, 384 });
		break;

	case 1:

		CreatePickup("alpha", { 2128, 2448 });
		CreatePickup("chi", { 528, 3024 });
		CreatePickup("rho", { 2960, 784 });
		CreatePickup("eta", { 656, 1936 });
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
