#ifndef __ENTITY_ENEMY_H__
#define __ENTITY_ENEMY_H__

#include "Point.h"
#include "External/PugiXml/src/pugixml.hpp"
#include "List.h"
#include "Module.h"
#include "Pickups.h"
#include "DynArray.h"

struct SDL_Texture;
struct Collider;
struct AnimationInfo;

enum class PathMovement;

enum class EnemyType 
{
	UNKNOWN = 0,
	W_SOUL,
	F_SOUL
};



struct Enemy
{
	EnemyType		type;

	iPoint			position;
	iPoint			target;

	fPoint velocity;
	int range;
	int safeZone;

	SDL_Rect		hitbox;
	Collider*		collider;


	bool fall = false;
	bool dead = false;

	PathMovement direction;

	bool pathCreated = false;

	DynArray<iPoint>* path;

	float gravity;

	AnimationInfo	animInfo;

	void Pathfind();
	void DebugRange();
	void Update(float dt);

	void Move(DynArray<iPoint>& path, float dt);
	void OnCollision(Collider* A, Collider* B);
};

class EntityEnemy : public Module
{

public:

	//Constructor
	EntityEnemy();

	// Destructor
	virtual ~EntityEnemy();

	//Called at first
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	void DoUpdate(float dt);

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
