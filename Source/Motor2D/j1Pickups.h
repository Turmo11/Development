#ifndef __j1PICKUPS_H__
#define __j1PICKUPS_H__

#include "p2Point.h"
#include "Dependencies/PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "j1Module.h"

struct SDL_Texture;
struct Collider;

struct AnimationInfo
{
	int			i = 0;
	p2SString	prev_s_anim_name = "shine";
	float		frame_count = 1;
};

struct Goal
{
	iPoint			position;
	AnimationInfo	anim_info;
};

struct Pickup
{
	p2SString		name;

	iPoint			position;
	SDL_Rect		pickup_hitbox;
	Collider*		pickup_collider;

	bool			collected;

	AnimationInfo	anim_info;

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

	void CreatePickup(p2SString name, iPoint position);
	void SetUp(int level);
	void OnCollision(Collider* A, Collider* B);
	void DrawAnimations();
	void GetCollected();
	void SetGoal(iPoint position);
	void UpdateGoal();
public:

	p2List<Pickup*>	pickup_list;
	
	p2List<Goal*> goal_list;
};

#endif
