/*#include "EntityManager.h"
#include "EntityPlayer.h"
#include "Pickups.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager()
{
	name.Create("entityManager");
}

EntityManager::~EntityManager()
{

}

bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	
	return true;
}

bool EntityManager::Start()
{
	return true;
}

bool EntityManager::PreUpdate()
{
	return true;
}

bool EntityManager::Update(float dt)
{
	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle) doLogic = true;
	UpdateAll(dt, doLogic);
	if (doLogic == true)
	{
		accumulatedTime = 0.0f;
		doLogic = false;
	}
	return true;
}

bool EntityManager::PostUpdate()
{
	return true;
}

bool EntityManager::CleanUp()
{
	if (!active) return true;

	return true;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* ret = nullptr;

	switch (type)
	{
	case EntityType::PLAYER: 
		ret = new Player();  break;
	case EntityType::ENEMY:
		ret = new Enemy();  break;
	case EntityType::ITEM:
		ret = new Item();  break;
	default: break;
	}

	
	if (ret != nullptr) entities.Add(ret);

	return ret;
}*/