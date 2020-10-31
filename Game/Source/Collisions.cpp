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
	List_item<Collider*>* collider_iterator = colliders.start;
	while (collider_iterator != nullptr) 
	{
		if (collider_iterator->data->toDelete == true)
		{
			colliders.del(collider_iterator);
		}

		collider_iterator = collider_iterator->next;
	}

	// Calculates collisions
	Collider* c1;
	Collider* c2;

	collider_iterator = colliders.start;
	while (collider_iterator != nullptr) 
	{
		c1 = collider_iterator->data;

		List_item<Collider*>* collider_iterator2 = colliders.start;

		while (collider_iterator2 != nullptr) 
		{

			c2 = collider_iterator2->data;

			if (c1 != c2) 
			{
				if (c1->CheckCollision(c2->rect) == true && (c1->type == ObjectType::PLAYER || c2->type == ObjectType::PLAYER))
				{
					if (c1->callBack)
						c1->callBack->OnCollision(c1, c2);

					if (c2->callBack)
						c2->callBack->OnCollision(c2, c1);
				}
			}
			collider_iterator2 = collider_iterator2->next;
		}
		collider_iterator = collider_iterator->next;
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
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
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
	List_item<Collider*>* collider_iterator = colliders.start;

	while (collider_iterator != nullptr) 
	{

		switch (collider_iterator->data->type)
		{
		case ObjectType::PLAYER: // yellow
			App->render->DrawQuad(collider_iterator->data->rect, 255, 255, 0, alpha);
			break;
		case ObjectType::GROUND: // light green
			App->render->DrawQuad(collider_iterator->data->rect, 194, 255, 198, alpha);
			break;
		case ObjectType::PLATFORM: // light blue
			App->render->DrawQuad(collider_iterator->data->rect, 85, 170, 255, alpha);
			break;
		case ObjectType::LETTER: // pink
			App->render->DrawQuad(collider_iterator->data->rect, 255, 0, 255, alpha);
			break;
		case ObjectType::ENEMY: // purple
			App->render->DrawQuad(collider_iterator->data->rect, 85, 0, 255, alpha);
			break;
		case ObjectType::GOAL: // red
			App->render->DrawQuad(collider_iterator->data->rect, 255, 0, 0, alpha);
			break;
		case ObjectType::DEATH: // black
			App->render->DrawQuad(collider_iterator->data->rect, 0, 0, 0, alpha);
			break;

		}
		collider_iterator = collider_iterator->next;
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
	List_item<ObjectGroup*>* list_i = App->map->data.objectGroups.start;
	while (list_i != nullptr) {
		for (int i = 0; i < list_i->data->objectsSize; i++) {

			AddCollider(*list_i->data->objects[i].collider, list_i->data->objects[i].type, nullptr, &list_i->data->objects[i].properties);
		}
		list_i = list_i->next;
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