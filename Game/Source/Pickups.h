#ifndef __PICKUPS_H__
#define __PICKUPS_H__

#include "p2Point.h"
#include "External/PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "Module.h"

struct SDL_Texture;
struct Collider;

struct AnimationInfo
{
	int			i = 0;
	p2SString	prevSAnimName = "shine";
	float		frameCount = 1;
};

struct Goal
{
	iPoint			position;
	AnimationInfo	animInfo;
};

struct Pickup
{
	p2SString		name;

	iPoint			position;
	SDL_Rect		pickupHitbox;
	Collider*		pickupCollider;

	bool			collected;

	AnimationInfo	animInfo;

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

	void CreatePickup(p2SString name, iPoint position);
	void OnCollision(Collider* A, Collider* B);
	void DrawAnimations();
	void GetCollected();
	void SetGoal(iPoint position);
	void UpdateGoal();
	void DebugCollectAll();

public:

	p2List<Pickup*>	pickupList;
	
	p2List<Goal*> goalList;
};

#endif
