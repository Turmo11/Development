#ifndef __j1PICKUPS_H__
#define __j1PICKUPS_H__

#include "p2Point.h"
#include "Dependencies/PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "j1Module.h"

struct SDL_Texture;
struct Collider;

struct Pickups
{

	//Animation
	SDL_Rect pickup_hitbox;
	Collider* pickup_collider;

	bool dead;

};

class j1Pickups : public j1Module
{

public:

	//Constructor
	j1Pickups();

	// Destructor
	virtual ~j1Pickups();

	//Called at first
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	void j1Pickups::OnCollision(Collider* A, Collider* B);

public:

	Pickups pickup;
	

};

#endif
