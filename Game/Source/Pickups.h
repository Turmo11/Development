#ifndef __PICKUPS_H__
#define __PICKUPS_H__

#include "Point.h"
#include "External/PugiXml/src/pugixml.hpp"
#include "List.h"
#include "Module.h"

struct SDL_Texture;
struct Collider;

enum class PickupType
{
	LETTER,
	HEALTH
};

struct AnimationInfo
{
	int			i = 0;
	SString	prevSAnimName;
	float		frameCount = 1;
	bool flip = false;
};

struct Goal
{
	iPoint position;
	AnimationInfo animInfo;
};

struct Pickup
{
	SString name;
	PickupType type;

	iPoint position;
	SDL_Rect pickupHitbox;
	Collider* pickupCollider;

	bool collected;

	AnimationInfo animInfo;

};

class Pickups : public Module
{

public:

	//Constructor
	Pickups();

	// Destructor
	virtual ~Pickups();

	//Called at first
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	void CreatePickup(PickupType type, SString name, iPoint position);
	void OnCollision(Collider* A, Collider* B);
	void DrawAnimations();
	void GetCollected();
	void SetGoal(iPoint position);
	void UpdateGoal();
	void DebugCollectAll();

public:

	List<Pickup*> pickupList;
	
	List<Goal*> goalList;
};

#endif
