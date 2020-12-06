#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "Point.h"
#include "List.h"
#include "Module.h"
#include "External\SDL\include\SDL_rect.h"
#include "SString.h"
#include "Pickups.h"

struct Collider;


struct Beam
{
	AnimationInfo animInfo;
	AnimationInfo cdInfo;

	float velocity;
	iPoint position;
	Collider* beamCollider;
	SDL_Rect beamHitbox;

	void UpdatePos()
	{
		position.x += velocity;
	}

};

class Projectile : public Module 
{
public:

	//Constructor
	Projectile();

	// Destructor
	virtual ~Projectile();

	//Called at first
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void SpawnBeam(bool flip);
	void OnCollision(Collider* A, Collider* B);
	void DrawAnimations();

	void DoUpdate(float dt);

	void ShowCd();

public:

	List<Beam*> beamList;
	SString name;
	Beam* testBeam;

	SDL_Texture* uiBeamTex;

	bool active = false;
	bool showCd = false;

	AnimationInfo animInfoYellow;
	
};

#endif
