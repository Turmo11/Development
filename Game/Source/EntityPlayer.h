
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "External/PugiXml/src/pugixml.hpp"
#include "List.h"
#include "Point.h"
#include "Module.h"
#include "External\SDL\include\SDL_rect.h"

struct Collider;

enum class PlayerStates {

	IDLE = 0,
	RUNNING,
	JUMP,
	FALL,
	CROUCH,
	STRIKE,
	DEBUG
};

struct Player 
{
	PlayerStates currentState;

	Point<float> position;
	Point<float> speed;
	Point<float> maxSpeed;
	Point<float> acceleration;
	
	SString animation;

	float gravity;

	//Booleans
	bool movingRight;
	bool movingLeft;
	bool jumping;
	bool grounded;
	bool collidingWall;
	bool ableToDrop;

	bool disabled;		//used during screen transitions or deaths when we want to take away the control from the player
	bool locked;			//used during cutscenes when we want to take away the control from the player
	bool godMode;
	bool ascending;
	bool shielded;

	bool flip;			 //used to handle texture flips

	//Collider
	int hitboxWidth;
	int hitboxHeight;
	SDL_Rect playerHitbox;
	Collider* playerCollider;
};

// ----------------------------------------------------
class EntityPlayer : public Module
{
public:

	//Constructor
	EntityPlayer();

	// Destructor
	virtual ~EntityPlayer();

	//Save and Load
	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);

	//Called at first
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before render is available

	bool Awake(pugi::xml_node& conf);

	// Called before quitting
	bool CleanUp();


public:

	//Movement
	void HorizontalMovement(float dt);
	void MoveRight(float dt);
	void MoveLeft(float dt);
	void MoveUp(float dt);
	void MoveDown(float dt);

	void LoadSoundFx();

	void GodMode();
	bool SummonPlayer();
	void ResetPlayer();
	void TakeLife();
	void AddLife();
	void Ascend(float time = 2.0f);

	void ShowShieldUI(bool active);

	bool CheckAirborne();

	void EntityPlayer::OnCollision(Collider* A, Collider* B);

	void SetCamera();
	

public:

	Player player;
	bool addLife;

	SDL_Texture* shieldTex;

	//Sound
	uint jumpSound;
	uint beamSound;
	uint healthSound;
	uint pickupSound;
	uint hitSound;
	uint deathSound;
	uint checkSound;

private:

	enum class Ascending
	{
		NONE,
		ASCENDING,
		ASCENDED

	} currentStep = Ascending::NONE;

	float xAxis, yAxis;
	Uint32		startTime = 0;
	Uint32		totalTime = 0;

	float projectileCooldown;
	float checkpointCooldown;
	float projectileTimer = 0.0f;
	float checkpointTimer = 0.0f;

	float shieldCooldown;
	float shieldDuration;
	float shieldTimer = 0.0f;

	bool showShieldCd;
	bool blinkShield;
	float accumulatedTime;
	bool showShieldUi;

};



#endif // __Player_H__