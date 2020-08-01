#include "j1App.h"
#include "j1Collisions.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Input.h"

j1Collisions::j1Collisions() : j1Module(), debug_colliders(false)
{
	name.create("collisions");

}

bool j1Collisions::Init() {

	return true;
};

bool j1Collisions::Awake(pugi::xml_node& config) {


	return true;
};

bool j1Collisions::Start() {

	debug_colliders = true;
	LoadFromMap();

	return true;
};

bool j1Collisions::PreUpdate() {

	//Deletes any collider waiting to be destroyed (to_delete == true)
	p2List_item<Collider*>* collider_iterator = colliders.start;
	while (collider_iterator != nullptr) {

		if (collider_iterator->data->to_delete == true)
		{
			colliders.del(collider_iterator);
		}

		collider_iterator = collider_iterator->next;
	}

	// Calculates collisions
	Collider* c1;
	Collider* c2;

	collider_iterator = colliders.start;
	while (collider_iterator != nullptr) {

		c1 = collider_iterator->data;

		p2List_item<Collider*>* collider_iterator2 = colliders.start;

		while (collider_iterator2 != nullptr) {

			c2 = collider_iterator2->data;

			if (c1 != c2) {
				if (c1->CheckCollision(c2->rect) == true && (c1->type == object_type::PLAYER || c2->type == object_type::PLAYER))
				{
					if (c1->callback)
						c1->callback->OnCollision(c1, c2);

					if (c2->callback)
						c2->callback->OnCollision(c2, c1);
				}
			}
			collider_iterator2 = collider_iterator2->next;
		}
		collider_iterator = collider_iterator->next;
	}
	return true;
};

bool j1Collisions::Update(float dt) {

	DebugDraw();
	return true;
};

void j1Collisions::DebugDraw() {
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		if (debug_colliders)
		{
			debug_colliders = false;
		}
		else
		{
			debug_colliders = true;
		}
	}
	if (debug_colliders == false)
		return;

	Uint8 alpha = 80; 	//Alpha value for all debug colliders
	p2List_item<Collider*>* collider_iterator = colliders.start;
	while (collider_iterator != nullptr) {

		switch (collider_iterator->data->type)
		{
		case object_type::PLAYER: // yellow
			App->render->DrawQuad(collider_iterator->data->rect, 255, 255, 0, alpha);
			break;
		case object_type::GROUND: // light green
			App->render->DrawQuad(collider_iterator->data->rect, 194, 255, 198, alpha);
			break;
		case object_type::PLATFORM: // light blue
			App->render->DrawQuad(collider_iterator->data->rect, 85, 170, 255, alpha);
			break;
		case object_type::LETTER: // pink
			App->render->DrawQuad(collider_iterator->data->rect, 255, 0, 255, alpha);
			break;
		case object_type::GOAL: // red
			App->render->DrawQuad(collider_iterator->data->rect, 255, 0, 0, alpha);
			break;
		case object_type::DEATH: // black
			App->render->DrawQuad(collider_iterator->data->rect, 0, 0, 0, alpha);
			break;

		}
		collider_iterator = collider_iterator->next;
	}

}

bool j1Collisions::PostUpdate() {

	return true;
};

bool j1Collisions::CleanUp() {

	return true;
};


Collider* j1Collisions::AddCollider(SDL_Rect rect, object_type type, j1Module* callback, Properties* userdata)
{
	Collider* ret = new Collider;

	ret->callback = callback;
	ret->rect = rect;
	ret->type = type;
	ret->userdata = userdata;

	colliders.add(ret);

	return ret;
}


//Loads colliders from tiled map
void j1Collisions::LoadFromMap() {
	p2List_item<ObjectGroup*>* list_i = App->map->data.object_groups.start;
	while (list_i != nullptr) {
		for (int i = 0; i < list_i->data->objects_size; i++) {

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

Collider::Collider(Object object) {
	rect = *object.collider;
	type = object.type;
}