/*#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Application.h"
#include "Render.h"
#include "Textures.h"

#include "Point.h"
#include "Log.h"

class Player;

enum class EntityType
{
	PLAYER,
	PICKUPS,
	ENEMIES
};

class Entity
{

public:

	Entity(EntityType type) : type(type), active(true) {}	
	virtual ~Entity();

	virtual bool Update(float dt)
	{
		return true;
	}


	fPoint position;

public:

	EntityType type;
	bool active = true;
};

#endif*/