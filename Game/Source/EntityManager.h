/*#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "List.h"
#include "Point.h"


#include <vector>

class EntityManager : public Module
{

public: 

	EntityManager();
	virtual ~EntityManager();

	bool Awake(pugi::xml_node&);
	bool Start();

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	bool CleanUp();

	bool UpdateAll(float dt, bool doLogic);

	Entity* CreateEntity(EntityType type);
	void DestroyEntity(Entity* entity);

public:

	List<Entity*> entities;
	float accumulatedTime = 0.0f;
	float updateMsCycle = 0.0f;
	bool doLogic = false;

};

#endif
*/