#ifndef __j1WALKINGENEMY_H__
#define __j1WALKINGENEMY_H__

#include "p2Point.h"
#include "Dependencies/PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "j1Module.h"
#include "j1Pickups.h"

struct SDL_Texture;
struct Collider;
struct AnimationInfo;

enum class enemy_type 
{
	UNKNOWN = 0,
	SOUL
};



struct Enemy
{
	enemy_type		type;

	iPoint			position;
	SDL_Rect		hitbox;
	Collider*		collider;

	bool			dead;

	AnimationInfo	anim_info;

};

class j1WalkingEnemy : public j1Module
{

public:

	//Constructor
	j1WalkingEnemy();

	// Destructor
	virtual ~j1WalkingEnemy();

	//Called at first
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	void CreateEnemy(enemy_type type, iPoint position);
	void OnCollision(Collider* A, Collider* B);
	void DrawAnimations();
	void GetKilled();

public:

	p2List<Enemy*>	enemy_list;


};

#endif
