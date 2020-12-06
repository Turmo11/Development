#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Render.h"
#include "Map.h"
#include "Collisions.h"
#include "Scene.h"
#include "EntityEnemy.h"
#include "EntityPlayer.h"
#include "Pathfinding.h"
#include "FadeToBlack.h"


EntityEnemy::EntityEnemy() : Module()
{
	name.Create("enemy");
}


// Destructor
EntityEnemy::~EntityEnemy()
{}

bool EntityEnemy::Start()
{


	return true;
}

bool EntityEnemy::PreUpdate()
{
	return true;
}

bool EntityEnemy::Update(float dt)
{
	if (app->fadeToBlack->activeScene == "Scene")
	{
		DoUpdate(dt);
		DrawAnimations();
	}
	return true;
}
void EntityEnemy::DoUpdate(float dt)
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

bool EntityEnemy::PostUpdate()
{
	return true;
}

bool EntityEnemy::CleanUp()
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

void EntityEnemy::CreateEnemy(EnemyType type, iPoint position)
{
	Enemy* newEnemy = new Enemy;

	newEnemy->type = type;
	newEnemy->position = position;

	if (type == EnemyType::W_SOUL)
	{
		newEnemy->hitbox.x = position.x + 16;
		newEnemy->hitbox.y = position.y + 32;
		newEnemy->hitbox.w = 64;
		newEnemy->hitbox.h = 75;
	}
	else if (type == EnemyType::F_SOUL)
	{
		newEnemy->hitbox.x = position.x - 4;
		newEnemy->hitbox.y = position.y + 10;
		newEnemy->hitbox.w = 38;
		newEnemy->hitbox.h = 38;
	}

	newEnemy->collider = app->collisions->AddCollider(newEnemy->hitbox, ObjectType::ENEMY, this);

	newEnemy->dead = false;

	newEnemy->velocity.x = 100.0f;
	newEnemy->velocity.y = 150.0f;
	newEnemy->gravity = 150.0f;
	newEnemy->range = 500;
	newEnemy->safeZone = 0;

	enemyList.add(newEnemy);
}

void EntityEnemy::DrawAnimations()
{
	ListItem<Enemy*>* enemyIterator = enemyList.start;

	while (enemyIterator != NULL)
	{
		if (!enemyIterator->data->dead)
		{
			if (enemyIterator->data->type == EnemyType::W_SOUL)
			{
				app->map->DrawStaticAnimation("soul", "soul_enemy", enemyIterator->data->position, &enemyIterator->data->animInfo);
			}
			if (enemyIterator->data->type == EnemyType::F_SOUL)
			{
				app->map->DrawStaticAnimation("soul", "soul_enemyx64", enemyIterator->data->position, &enemyIterator->data->animInfo);
			}

		}
		enemyIterator = enemyIterator->next;
	}
}

void EntityEnemy::OnCollision(Collider* A, Collider* B)
{
	ListItem<Enemy*>* enemyIterator = enemyList.start;

	while (enemyIterator != NULL)
	{
		if (!enemyIterator->data->collider->toDelete)
		{
			enemyIterator->data->OnCollision(A, B);
		}
		enemyIterator = enemyIterator->next;
	}

}

void Enemy::OnCollision(Collider* A, Collider* B)
{
	// ------------ Player Colliding with the ground ------------------
	if (A->type == ObjectType::ENEMY && B->type == ObjectType::GROUND) {

		//Colliding from above
		if (A->rect.y + A->rect.h - velocity.y - 2 < B->rect.y
			&& A->rect.x < B->rect.x + B->rect.w
			&& A->rect.x + A->rect.w > B->rect.x)
		{
			if (velocity.y > 0)
			{
				velocity.y = 0;
			}

			position.y = B->rect.y - collider->rect.h - 20;
		}
		//Colliding from the sides
		else if (A->rect.y + (A->rect.h * 1.0f / 4.0f) < B->rect.y + B->rect.h
			&& A->rect.y + (A->rect.h * 3.0f / 4.0f) > B->rect.y)
		{

			if ((A->rect.x + A->rect.w) < (B->rect.x + B->rect.w / 4))
			{ //Player to the left 
				position.x = B->rect.x - A->rect.w - 32;
			}
			else if (A->rect.x > (B->rect.x + B->rect.w * 3 / 4))
			{ //Player to the right
				position.x = B->rect.x + B->rect.w + 20;
			}

		}
		//from below
		else if (A->rect.y < (B->rect.y + B->rect.h))
		{
			velocity.y = 0;
			position.y = B->rect.y + B->rect.h;
		}
	}

}

void EntityEnemy::GetKilled()
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

	if (type == EnemyType::W_SOUL)
	{
		collider->SetPos(position.x + 16, position.y + 32);
	}
	else if (type == EnemyType::F_SOUL)
	{
		collider->SetPos(position.x - 4, position.y + 10);
	}
}
void Enemy::DebugRange()
{
	int diameter = range * 2;
	app->render->DrawQuad({ position.x - range + (collider->rect.w / 2), position.y - range, diameter, diameter }, 255, 0, 0, 255, false);
	app->render->DrawQuad({ position.x - safeZone + (hitbox.w / 2), position.y - safeZone + (hitbox.h / 2), safeZone * 2, safeZone * 2 }, 0, 255, 0, 255, false);

}

void Enemy::Move(DynArray<iPoint>& path, float dt)
{
	if (path.Count() > 2)
	{
		iPoint tile = path[0];
		iPoint nextTile = path[1];

		int xDiff = nextTile.x - tile.x;
		int yDiff = nextTile.y - tile.y;

		if (xDiff != 0 || yDiff != 0)
		{
			direction = app->pathfinding->CheckDirection(path, direction);
		}
	}
	else
	{
		direction = app->pathfinding->CheckDirection(path, direction);
	}

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
		position.x += 1.5f * velocity.x * dt;

		break;
	case PathMovement::NO_MOVE:
		position = position;

		break;
	}
}

void Enemy::Pathfind()
{
	target = { (int)app->player->player.position.x, (int)app->player->player.position.y };

	if (target.DistanceTo(position) < range)
	{
		iPoint origin = { app->map->WorldToMap((int)position.x, (int)position.y) };
		iPoint destination /*= { app->map->WorldToMap((int)target.x, (int)target.y + 64) }*/;

		if (position.x < target.x)
		{
			destination = { app->map->WorldToMap((int)target.x + 32, (int)target.y + 64) };
		}
		else if (position.x > target.x)
		{
			destination = { app->map->WorldToMap((int)target.x + 64, (int)target.y + 64) };
		}
		/*if (position.x < target.x)
		{
			destination = { app->map->WorldToMap((int)(target.x + app->player->player.playerHitbox.x), (int)(target.y + app->player->player.playerHitbox.y / 2)) };
		}
		else
		{
			destination = { app->map->WorldToMap((int)(target.x), (int)(target.y + app->player->player.playerHitbox.y)) };
		}*/

		if (target.DistanceTo(position) > safeZone)
		{
			path = app->pathfinding->CreatePath(origin, destination);
		}
		else
		{
			LOG("SAFE ZONE");
		}
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


