#ifndef __WALKING_ENEMY_H__
#define __WALKING_ENEMY_H__

#include "Point.h"
#include "External/PugiXml/src/pugixml.hpp"
#include "List.h"
#include "Module.h"
#include "Pickups.h"

struct SDL_Texture;
struct Collider;
struct AnimationInfo;

enum class EnemyType 
{
	UNKNOWN = 0,
	SOUL
};



struct Enemy
{
	EnemyType		type;

	iPoint			position;
	SDL_Rect		hitbox;
	Collider*		collider;

	bool			dead;

	AnimationInfo	animInfo;

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

	void CreateEnemy(EnemyType type, iPoint position);
	void OnCollision(Collider* A, Collider* B);
	void DrawAnimations();
	void GetKilled();

public:

	List<Enemy*>	enemyList;


};

#endif
