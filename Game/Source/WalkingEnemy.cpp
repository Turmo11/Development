#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Collisions.h"
#include "Scene.h"
#include "WalkingEnemy.h"


WalkingEnemy::WalkingEnemy() : Module()
{
	name.create("enemy");
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
	p2List_item<Enemy*>* item;
	item = enemy_list.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	enemy_list.clear();
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

	new_enemy->collider = App->collisions->AddCollider(new_enemy->hitbox, ObjectType::ENEMY, this);

	new_enemy->dead = false;

	enemy_list.add(new_enemy);

}

void WalkingEnemy::DrawAnimations()
{
	p2List_item<Enemy*>* enemy_iterator = enemy_list.start;

	while (enemy_iterator != NULL)
	{
		if (!enemy_iterator->data->dead)
		{
			if (enemy_iterator->data->type == EnemyType::SOUL)
			{
				App->map->DrawStaticAnimation("soul", "soul_enemyx64", enemy_iterator->data->position, &enemy_iterator->data->anim_info);
			}

		}
		enemy_iterator = enemy_iterator->next;
	}
}

void WalkingEnemy::OnCollision(Collider* A, Collider* B)
{
	if (A->type == ObjectType::ENEMY && B->type == ObjectType::PLAYER)
	{
		A->to_delete = true;
		GetKilled();
	}
}

void WalkingEnemy::GetKilled()
{
	p2List_item<Enemy*>* enemy_iterator = enemy_list.start;

	while (enemy_iterator != NULL)
	{
		if (enemy_iterator->data->collider->to_delete)
		{
			enemy_iterator->data->dead = true;
			
		}
		enemy_iterator = enemy_iterator->next;
	}
}