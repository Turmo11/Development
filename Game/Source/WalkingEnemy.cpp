#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Collisions.h"
#include "Scene.h"
#include "WalkingEnemy.h"
#include "EntityPlayer.h"
#include "Pathfinding.h"


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
	DoUpdate(dt);
	DrawAnimations();

	return true;
}
void WalkingEnemy::DoUpdate(float dt)
{
	ListItem<Enemy*>* enemyIterator = enemyList.start;

	while (enemyIterator != NULL)
	{
		if (!enemyIterator->data->dead)
		{
			enemyIterator->data->Update(dt);
		}
		enemyIterator = enemyIterator->next;
	}
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
	Enemy* newEnemy = new Enemy;

	newEnemy->type = type;
	newEnemy->position = position;

	newEnemy->hitbox.x = position.x + 16;
	newEnemy->hitbox.y = position.y + 32;
	newEnemy->hitbox.w = 64;
	newEnemy->hitbox.h = 75;

	/*newEnemy->hitbox.x = position.x - 4;
	newEnemy->hitbox.y = position.y + 10;
	newEnemy->hitbox.w = 38;
	newEnemy->hitbox.h = 38;*/

	newEnemy->collider = app->collisions->AddCollider(newEnemy->hitbox, ObjectType::ENEMY, this);

	newEnemy->dead = false;

	newEnemy->velocity.x = 100.0f;
	newEnemy->velocity.y = 100.0f;
	newEnemy->gravity = 150.0f;
	newEnemy->range = 500;

	newEnemy->adjust = 2;
	newEnemy->adjustCollider = 20;
	newEnemy->adjustPath = { 7, 6 };


	enemyList.add(newEnemy);

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
				app->map->DrawStaticAnimation("soul", "soul_enemy", enemyIterator->data->position, &enemyIterator->data->animInfo);
			}

		}
		enemyIterator = enemyIterator->next;
	}
}

void WalkingEnemy::OnCollision(Collider* A, Collider* B)
{
	/*if (A->type == ObjectType::ENEMY && B->type == ObjectType::GROUND)
	{
		if (A->rect.y + A->rect.h >= B->rect.y && A->rect.y < B->rect.y)
		{
			if (fall)
			{
				fall = false;
				dead = true;

				if (collider != nullptr)
					collider->to_delete = true;
				collider = nullptr;

				position.y = B->rect.y + B->rect.h - adjustCollider;

				ColUp = false;
				ColDown = true;
			}
		}
	}*/
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

void Enemy::Update(float dt)
{
	Pathfind();
	if (app->map->debug)
	{
		DebugRange();
	}

	collider->SetPos(position.x + 16, position.y + 32);
	LOG("Velocity = %f", velocity.x);
}
void Enemy::DebugRange()
{
	int diameter = range * 2;
	app->render->DrawQuad({ position.x - range + (collider->rect.w / 2), position.y - range, diameter, diameter }, 255, 0, 0, 255, false);
}

void Enemy::Move(DynArray<iPoint>& path, float dt)
{
	direction = app->pathfinding->CheckDirection(path);

	switch (direction)
	{
	case PathMovement::UP_LEFT:
		position.x -= velocity.x * dt;
		position.y -= velocity.y * dt;

		break;

	case PathMovement::UP_RIGHT:
		position.x += velocity.x * dt;
		position.y -= velocity.y * dt;

		break;

	case PathMovement::DOWN_LEFT:
		position.x -= velocity.x * dt;
		position.y += velocity.y * dt;

		break;

	case PathMovement::DOWN_RIGHT:
		position.x += velocity.x * dt;
		position.y += velocity.y * dt;

		break;

	case PathMovement::UP:
		position.y -= velocity.y * dt;

		break;

	case PathMovement::DOWN:
		position.y += velocity.y * dt;

		break;

	case PathMovement::LEFT:
		position.x -= velocity.x * dt;

		break;

	case PathMovement::RIGHT:
		position.x += velocity.x * dt;

		break;
	}
}

void Enemy::Pathfind()
{
	target = { (int)app->player->player.position.x, (int)app->player->player.position.y };

	if (target.DistanceTo(position) < range)
	{
		iPoint origin = { app->map->WorldToMap((int)position.x, (int)position.y) };
		iPoint destination = { app->map->WorldToMap((int)target.x, (int)target.y) };

		/*if (position.x < app->player->player.position.x)
		{
			destination = { app->map->WorldToMap((int)(app->player->player.position.x + app->player->player.playerHitbox.x), (int)(app->player->player.position.y + app->player->player.playerHitbox.y / 2)) };
		}
		else
		{
			destination = { app->map->WorldToMap((int)(app->player->player.position.x), (int)(app->player->player.position.y + app->player->player.playerHitbox.y)) };
		}*/

		path = app->pathfinding->CreatePath(origin, destination);
		if (path != NULL)
		{
			Move(*path, app->GetDt());
			pathCreated = true;
		}
		else
		{
			pathCreated = false;
		}
	}
	else if (pathCreated)
	{
		path->Clear();
		pathCreated = false;
	}



////Pathfinding -------------------------------------------
//target = { (int)app->player->player.position.x, (int)app->player->player.position.y };


//if (target.DistanceTo(position) < range) //Detection area
//{
//	//Find the closest tile to current position
//	app->pathfinding->CreatePath(app->map->WorldToMap(position.x, position.y), app->map->WorldToMap(target.x, target.y));

//	const DynArray<iPoint>* Path = app->pathfinding->GetLastPath();

//	const iPoint* tile;
//	if (Path->Count() != 0) {
//		if (Path->Count() > 2) {
//			tile = Path->At(2);
//		}
//		else if (Path->Count() > 1) {
//			tile = Path->At(1);
//		}
//		else
//		{
//			tile = Path->At(0);
//		}

//		iPoint closest_center = app->map->MapToWorldCentered(tile->x, tile->y);

//		float dt = app->GetDt();

//		if (closest_center.x > position.x + collider->rect.w / 2)
//		{
//			position.x += 2 * (velocity.x * dt);
//			LOG("PLAYER TO THE RIGHT");

//		}
//		else if (closest_center.x < position.x + collider->rect.w / 2)
//		{
//			position.x -= (velocity.x * dt);
//			LOG("PLAYER TO THE LEFT");
//		}
//	}
//}

}
