#include "Application.h"
#include "Collisions.h"
#include "Map.h"
#include "Render.h"
#include "Input.h"
#include "Log.h"

Collisions::Collisions() : Module(), debugColliders(false)
{
	name.Create("collisions");
}

bool Collisions::Init()
{
	return true;
};

bool Collisions::Awake(pugi::xml_node& config) 
{
	return true;
};

bool Collisions::Start() 
{
	debugColliders = false;
	LoadFromMap();

	return true;
};

bool Collisions::PreUpdate() 
{
	//Deletes any collider waiting to be destroyed (toDelete == true)
	ListItem<Collider*>* colliderIterator = colliders.start;
	while (colliderIterator != nullptr) 
	{
		if (colliderIterator->data->toDelete == true)
		{
			colliders.del(colliderIterator);
		}

		colliderIterator = colliderIterator->next;
	}

	// Calculates collisions
	Collider* c1;
	Collider* c2;

	colliderIterator = colliders.start;
	while (colliderIterator != nullptr) 
	{
		c1 = colliderIterator->data;

		ListItem<Collider*>* colliderIterator2 = colliders.start;

		while (colliderIterator2 != nullptr) 
		{

			c2 = colliderIterator2->data;

			if (c1 != c2) 
			{
				if (c1->CheckCollision(c2->rect) == true)
				{
					if (c1->callBack)
						c1->callBack->OnCollision(c1, c2);

					if (c2->callBack)
						c2->callBack->OnCollision(c2, c1);
				}
			}
			colliderIterator2 = colliderIterator2->next;
		}
		colliderIterator = colliderIterator->next;
	}

	return true;
};

bool Collisions::Update(float dt) 
{
	DebugDraw();
	//LOG("Collider mov test - Collider %d pos x = %d, y = %d", colliders.start->data->type, colliders.start->data->rect.x, colliders.start->data->rect.y);

	return true;
};

void Collisions::DebugDraw() 
{
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		if (debugColliders)
		{
			debugColliders = false;
		}
		else
		{
			debugColliders = true;
		}
	}

	if (debugColliders == false)
		return;

	Uint8 alpha = 80; 	//Alpha value for all debug colliders
	ListItem<Collider*>* colliderIterator = colliders.start;

	while (colliderIterator != nullptr) 
	{

		switch (colliderIterator->data->type)
		{
		case ObjectType::PLAYER: // yellow
			app->render->DrawQuad(colliderIterator->data->rect, 255, 255, 0, alpha);
			break;
		case ObjectType::PROJECTILE: // orange
			app->render->DrawQuad(colliderIterator->data->rect, 255, 205, 0, alpha);
			break;
		case ObjectType::GROUND: // light green
			app->render->DrawQuad(colliderIterator->data->rect, 194, 255, 198, alpha);
			break;
		case ObjectType::PLATFORM: // light blue
			app->render->DrawQuad(colliderIterator->data->rect, 85, 170, 255, alpha);
			break;
		case ObjectType::LETTER: // pink
			app->render->DrawQuad(colliderIterator->data->rect, 255, 0, 255, alpha);
			break;
		case ObjectType::HEALTH: // red
			app->render->DrawQuad(colliderIterator->data->rect, 255, 0, 0, alpha);
			break;
		case ObjectType::ENEMY: // purple
			app->render->DrawQuad(colliderIterator->data->rect, 85, 0, 255, alpha);
			break;
		case ObjectType::GOAL: // red
			app->render->DrawQuad(colliderIterator->data->rect, 255, 0, 0, alpha);
			break;
		case ObjectType::DEATH: // black
			app->render->DrawQuad(colliderIterator->data->rect, 0, 0, 0, alpha);
			break;
		case ObjectType::CHECKPOINT: // white
			app->render->DrawQuad(colliderIterator->data->rect, 255, 255, 255, alpha);
			break;

		}
		colliderIterator = colliderIterator->next;
	}
}

bool Collisions::PostUpdate() 
{
	return true;
};

bool Collisions::CleanUp() 
{
	return true;
};


Collider* Collisions::AddCollider(SDL_Rect rect, ObjectType type, Module* callBack, Properties* userData)
{
	Collider* ret = new Collider;

	ret->callBack = callBack;
	ret->rect = rect;
	ret->type = type;
	ret->userData = userData;

	colliders.add(ret);

	return ret;
}


//Loads colliders from tiled map
void Collisions::LoadFromMap() 
{
	ListItem<ObjectGroup*>* listIt = app->map->data.objectGroups.start;
	while (listIt != nullptr) 
	{
		for (int i = 0; i < listIt->data->objectsSize; i++) 
		{
			AddCollider(*listIt->data->objects[i].collider, listIt->data->objects[i].type, nullptr, &listIt->data->objects[i].properties);
		}
		listIt = listIt->next;
	}
}

//  Struct Collider Methods --------------------------------------
bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return ((r.x + r.w > rect.x) && (r.x < rect.x + rect.w) &&
		(r.y + r.h > rect.y) && (r.y < rect.y + rect.h));
}

Collider::Collider(Object object) 
{
	rect = *object.collider;
	type = object.type;
}