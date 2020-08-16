#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Collisions.h"
#include "j1Scene.h"
#include "j1WalkingEnemy.h"


j1WalkingEnemy::j1WalkingEnemy() : j1Module()
{
	name.create("enemy");
}


// Destructor
j1WalkingEnemy::~j1WalkingEnemy()
{}

bool j1WalkingEnemy::Start()
{


	return true;
}

bool j1WalkingEnemy::PreUpdate()
{
	return true;
}

bool j1WalkingEnemy::Update(float dt)
{
	DrawAnimations();

	return true;
}

bool j1WalkingEnemy::PostUpdate()
{
	return true;
}

bool j1WalkingEnemy::CleanUp()
{
	return true;
}

void j1WalkingEnemy::CreateEnemy(enemy_type type, iPoint position)
{
	Enemy* new_enemy = new Enemy;

	new_enemy->type = type;
	new_enemy->position = position;

	new_enemy->hitbox.x = position.x + 16;
	new_enemy->hitbox.y = position.y + 32;
	new_enemy->hitbox.w = 64;
	new_enemy->hitbox.h = 75; 

	new_enemy->collider = App->collisions->AddCollider(new_enemy->hitbox, object_type::ENEMY, this);

	new_enemy->dead = false;

	enemy_list.add(new_enemy);

}

void j1WalkingEnemy::DrawAnimations()
{
	p2List_item<Enemy*>* enemy_iterator = enemy_list.start;

	while (enemy_iterator != NULL)
	{
		if (!enemy_iterator->data->dead)
		{
			if (enemy_iterator->data->type == enemy_type::SOUL)
			{
				App->map->DrawStaticAnimation("soul", "soul_enemy", enemy_iterator->data->position, &enemy_iterator->data->anim_info);
			}

		}
		enemy_iterator = enemy_iterator->next;
	}
}

void j1WalkingEnemy::OnCollision(Collider* A, Collider* B)
{
	if (A->type == object_type::ENEMY && B->type == object_type::PLAYER)
	{
		A->to_delete = true;
		GetKilled();
	}
}

void j1WalkingEnemy::GetKilled()
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