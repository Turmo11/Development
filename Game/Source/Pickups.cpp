#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Collisions.h"
#include "Pickups.h"
#include "GameScene.h"
#include "EntityPlayer.h"
#include "EntityEnemy.h"
#include "Audio.h"
#include <ctime>


Pickups::Pickups() : Module()
{
	name.Create("pickups");
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
	ListItem<Pickup*>* item;
	item = pickupList.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	pickupList.clear();

	// Remove goals
	ListItem<Goal*>* item2;
	item2 = goalList.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	goalList.clear();

	return true;
}

void Pickups::CreatePickup(PickupType type, SString name, iPoint position)
{
	Pickup* newPickup = new Pickup;

	newPickup->name = name;
	newPickup->position = position;

	newPickup->pickupHitbox.x = position.x;
	newPickup->pickupHitbox.y = position.y;
	newPickup->pickupHitbox.w = 32;
	newPickup->pickupHitbox.h = 32;

	newPickup->collected = false;

	if (type == PickupType::LETTER)
	{

		newPickup->pickupCollider = app->collisions->AddCollider(newPickup->pickupHitbox, ObjectType::LETTER, this);
		newPickup->type = PickupType::LETTER;
	}
	else if (type == PickupType::HEALTH)
	{
		newPickup->pickupCollider = app->collisions->AddCollider(newPickup->pickupHitbox, ObjectType::HEALTH, this);
		newPickup->type = PickupType::HEALTH;
	}

	pickupList.add(newPickup);
}

void Pickups::DrawAnimations()
{
	ListItem<Pickup*>* pickupIterator = pickupList.start;

	while (pickupIterator != NULL)
	{
		if (!pickupIterator->data->collected)
		{
			if (pickupIterator->data->type == PickupType::LETTER)
			{
				app->map->DrawStaticAnimation(pickupIterator->data->name.GetString(), "letter_tileset", pickupIterator->data->position, &pickupIterator->data->animInfo);
			}
			if (pickupIterator->data->type == PickupType::HEALTH)
			{
				app->map->DrawStaticAnimation(pickupIterator->data->name.GetString(), "heart", pickupIterator->data->position, &pickupIterator->data->animInfo);
			}
		}
		pickupIterator = pickupIterator->next;
	}
}

void Pickups::OnCollision(Collider* A, Collider* B)
{
	if (A->type == ObjectType::LETTER && B->type == ObjectType::PLAYER)
	{
		A->toDelete = true;
		GetCollected();
		app->gameScene->CheckLevelProgress();
		app->audio->PlayFx(app->player->pickupSound);
		int newScore;
		srand(time(NULL));
		newScore = (rand() % 200) + 100;
		app->gameScene->AddScore(newScore);
	}
	if (A->type == ObjectType::HEALTH && B->type == ObjectType::PLAYER)
	{
		A->toDelete = true;
		GetCollected();
		app->player->addLife = true;
		app->audio->PlayFx(app->player->healthSound);

	}

}

void Pickups::GetCollected()
{
	ListItem<Pickup*>* pickupIterator = pickupList.start;

	while (pickupIterator != NULL)
	{
		if (pickupIterator->data->pickupCollider->toDelete)
		{
			pickupIterator->data->collected = true;
			LOG("Collected %s", pickupIterator->data->name.GetString());
		}
		pickupIterator = pickupIterator->next;
	}
}

void Pickups::DebugCollectAll()
{
	ListItem<Pickup*>* pickupIterator = pickupList.start;

	while (pickupIterator != NULL)
	{
		if (pickupIterator->data->type == PickupType::LETTER)
		{
			pickupIterator->data->collected = true;
			
		}
		pickupIterator = pickupIterator->next;
	}

	app->gameScene->CheckLevelProgress();
}


void Pickups::SetGoal(iPoint position)
{
	Goal* leftGoal = new Goal;
	Goal* rightGoal = new Goal;

	leftGoal->position = position;

	rightGoal->position.x = position.x + 192;
	rightGoal->position.y = position.y;

	goalList.add(leftGoal);
	goalList.add(rightGoal);
}

void Pickups::UpdateGoal()
{
	ListItem<Goal*>* goalIterator = goalList.start;

	while (goalIterator != NULL)
	{
		if (app->gameScene->levelCompleted)
		{
			app->map->DrawStaticAnimation("goal", "goal", goalIterator->data->position, &goalIterator->data->animInfo);

		}
		goalIterator = goalIterator->next;
	}
}


