#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Collisions.h"
#include "Scene.h"
#include "WalkingEnemy.h"


WalkingEnemy::WalkingEnemy() : Module()
{
	name.Create("enemy");
}


// Destructor
WalkingEnemy::~WalkingEnemy()
{}

bool WalkingEnemy::Start()
{


	return true;
}

bool WalkingEnemy::PreUpdate()
{
	return true;
}

bool WalkingEnemy::Update(float dt)
{
	DrawAnimations();

	return true;
}

bool WalkingEnemy::PostUpdate()
{
	return true;
}

bool WalkingEnemy::CleanUp()
{
	// Remove pickups
	ListItem<Enemy*>* item;
	item = enemyList.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	enemyList.clear();
	return true;
}

void WalkingEnemy::CreateEnemy(EnemyType type, iPoint position)
{
	Enemy* new_enemy = new Enemy;

	new_enemy->type = type;
	new_enemy->position = position;

/*	new_enemy->hitbox.x = position.x + 16;
	new_enemy->hitbox.y = position.y + 32;
	new_enemy->hitbox.w = 64;
	new_enemy->hitbox.h = 75;*/ 
	new_enemy->hitbox.x = position.x - 4;
	new_enemy->hitbox.y = position.y + 10;
	new_enemy->hitbox.w = 38;
	new_enemy->hitbox.h = 38;

	new_enemy->collider = app->collisions->AddCollider(new_enemy->hitbox, ObjectType::ENEMY, this);

	new_enemy->dead = false;

	enemyList.add(new_enemy);

}

void WalkingEnemy::DrawAnimations()
{
	ListItem<Enemy*>* enemyIterator = enemyList.start;

	while (enemyIterator != NULL)
	{
		if (!enemyIterator->data->dead)
		{
			if (enemyIterator->data->type == EnemyType::SOUL)
			{
				app->map->DrawStaticAnimation("soul", "soul_enemyx64", enemyIterator->data->position, &enemyIterator->data->animInfo);
			}

		}
		enemyIterator = enemyIterator->next;
	}
}

void WalkingEnemy::OnCollision(Collider* A, Collider* B)
{
	if (A->type == ObjectType::ENEMY && B->type == ObjectType::PLAYER)
	{
		A->toDelete = true;
		GetKilled();
	}
}

void WalkingEnemy::GetKilled()
{
	ListItem<Enemy*>* enemyIterator = enemyList.start;

	while (enemyIterator != NULL)
	{
		if (enemyIterator->data->collider->toDelete)
		{
			enemyIterator->data->dead = true;
			
		}
		enemyIterator = enemyIterator->next;
	}
}