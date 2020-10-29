
#ifndef __Player_H__
#define __Player_H__

#include "Dependencies/PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "Module.h"

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
	bool				colliding_wall;
	bool				able_to_drop;

	bool				disabled;		//used during screen transitions or deaths when we want to take away the control from the player
	bool				locked;			//used during cutscenes when we want to take away the control from the player
	bool				god_mode;
	bool				ascending;

	bool				flip;			 //used to handle texture flips

	//Collider
	int					hitbox_width;
	int					hitbox_height;
	SDL_Rect			player_hitbox;
	Collider*			player_collider;
};

// ----------------------------------------------------
class E_Player : public Module
{
public:

	//Constructor
	E_Player();

	// Destructor
	virtual ~E_Player();

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
	void HorizontalMovement();
	void MoveRight();
	void MoveLeft();
	void MoveUp();
	void MoveDown();

	void GodMode();
	bool SummonPlayer();
	void ResetPlayer();
	void Ascend(float time = 2.0f);

	bool CheckAirborne();

	void E_Player::OnCollision(Collider* A, Collider* B);

	void SetCamera();

public:

	Player player;

private:

	enum class ascending
	{
		NONE,
		ASCENDING,
		ASCENDED

	} current_step = ascending::NONE;

	float x_axis, y_axis;
	Uint32		start_time = 0;
	Uint32		total_time = 0;
};



#endif // __Player_H__