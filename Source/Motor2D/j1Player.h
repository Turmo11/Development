
#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "Dependencies/PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

struct Collider;

enum class player_states {

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
	player_states		current_state;

	p2Point<float>		position;
	p2Point<float>		speed;
	p2Point<float>		max_speed;
	p2Point<float>		acceleration;
	
	p2SString			animation;

	float				gravity;

	//Booleans
	bool				moving_right;
	bool				moving_left;
	bool				jumping;
	bool				grounded;

	bool				disabled;		//used during screen transitions or deaths when we want to take away the control from the player
	bool				god_mode;

	bool				flip;			//used to handle texture flips

	//Collider
	int					hitbox_width;
	int					hitbox_height;
	SDL_Rect			player_hitbox;
	Collider*			player_collider;
};

// ----------------------------------------------------
class j1Player : public j1Module
{
public:

	//Constructor
	j1Player();

	// Destructor
	virtual ~j1Player();

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
	void HorizontalMovement();
	void MoveRight();
	void MoveLeft();
	void MoveUp();
	void MoveDown();

	void GodMode();
	bool SummonPlayer();
	void ResetPlayer();

	bool CheckAirborne();

	void j1Player::OnCollision(Collider* A, Collider* B);

	void SetCamera();

public:

	Player player;

private:
	float x_axis, y_axis;
};



#endif // __j1PLAYER_H__