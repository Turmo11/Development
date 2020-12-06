#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "Module.h"
#include "External/SDL/include/SDL.h"

enum class ObjectType;
struct Object;
struct Properties;


struct Collider
{
	SDL_Rect rect;					//Collider box
	bool toDelete = false;			//deletes collider if true
	ObjectType type;				//enum class collider type defined in map.h
	Module* callBack = nullptr;	
	Properties* userData;

	Collider(SDL_Rect rectangle, ObjectType type, Module* callBack = nullptr) :
		rect(rectangle),
		type(type),
		callBack(callBack)
	{}

	Collider(Object object);

	Collider() {};

	inline void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	inline bool CheckCollision(const SDL_Rect& r) const;
};



class Collisions : public Module {
	
public:
	Collisions();

	bool Init();

	virtual ~Collisions() {};

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	Collider* AddCollider(SDL_Rect rect, ObjectType type, Module* callBack, Properties* userData = nullptr);
	void LoadFromMap(); //Loads colliders from tiled map
	

private:

	void DebugDraw(); //Draws the collider pressing F9
	
public:

	List<Collider*> colliders;
	bool debugColliders;

private:

};


#endif
