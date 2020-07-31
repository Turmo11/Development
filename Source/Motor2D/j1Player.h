
#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "Dependencies/PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"


struct SDL_Texture;
struct Collider;

enum class player_states {

	IDLE = 0,
	RUNNING,
	JUMP,
	CROUCH,
	STRIKE,
	DEBUG
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

	p2Point<int> getPos() const;

	// Called each loop iteration

	/*void DrawPlayer();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;*/

	void MoveL();
	void MoveR();

	void MovePlayer(float dt);


	// Called before quitting
	bool CleanUp();

	bool is_dead = false;
	bool camera_pos = false;
	bool jumping = false;

public:



private:

	player_states current_state;

	

	SDL_Rect hitbox = {};

	SDL_Texture* p_texture;


	int score = 0;
	int snake = 0;
	//int velocity = 30;
	

public:

	//int gravity;
	int speed;
	float jumpF = -2.5f;
	int p_pos_y;
	p2Point<int> pos;
	
};



#endif // __j1PLAYER_H__