#ifndef __WalkingEnemy_H__
#define __WalkingEnemy_H__

#include "p2Point.h"
#include "Dependencies/PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "Module.h"
#include "Pickups.h"

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

class WalkingEnemy : public Module
{

public:

	//Constructor
	WalkingEnemy();

	// Destructor
	virtual ~WalkingEnemy();

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