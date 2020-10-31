#ifndef __MAP_H__
#define __MAP_H__

#include "External/PugiXml/src/pugixml.hpp"
#include "List.h"
#include "Point.h"
#include "SString.h"
#include "Module.h"

struct AnimationInfo;
// ----------------------------------------------------
union value {
	const char*		vString;
	int				vInt;
	float			vFloat;
};

struct Properties //Properties
{
	struct Property
	{
		SString	name;
		value		data;
		
	};

	Properties::~Properties()															//Deletes every property and frees all allocated memory.
	{
		List_item<Property*>* item;
		item = propertyList.start;
		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}
		propertyList.clear();												//Clears propertyList by deleting all items in the list and freeing all allocated memory.
	}

	value Get(const char* name, value* defaultValue = nullptr) const;

	List<Property*>	propertyList;
};


//Objects for collisions
enum class ObjectType
{
	UNKNOWN = 0,
	PLAYER,
	GROUND,
	PLATFORM,
	LETTER,
	ENEMY,
	GOAL,
	DEATH
};

struct Object
{
	uint				id;
	SString			name;
	ObjectType			type;
	SDL_Rect*			collider;
	SDL_Texture*		texture;
	Properties			properties;
};

struct ObjectGroup
{
	uint				id;
	SString			name;
	Object*				objects;
	uint				objectsSize;
};

struct Animations
{
	SString			name = "idle";
	uint				id;				//Tile which is animated
	uint				nFrames;	//Number of frames of the animation
	uint*				frames;
	uint				speed;
};

// ----------------------------------------------------
struct MapLayer
{
	SString	name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	//Get id of tile in position x, y from data[] array
	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	SString			name;
	int					firstgid;
	int					tileWidth;
	int					tileHeight;
	int					margin;
	int					spacing;
	SDL_Texture*		texture;
	int					texWidth;
	int					texHeight;
	int					numTilesWidth;
	int					numTilesHeight;
	int					offsetX;
	int					offsetY;
	List<Animations*> animations;

	SDL_Rect* playerTileRect = new SDL_Rect;
	SDL_Rect* PlayerTileRect(uint tileId) {

		SDL_Rect* ret = playerTileRect;

		int numTWidth = texWidth / tileWidth;
		int numTHeight = texHeight / tileHeight;

		int x = tileId % numTWidth;
		int y = tileId / numTWidth;


		ret->x = x * tileWidth;
		ret->y = y * tileHeight;
		ret->w = tileWidth;
		ret->h = tileHeight;

		return ret;
	}

};

enum class MapTypes
{
	UNKNOWN = 0,
	ORTHOGONAL,
	ISOMETRIC,
	STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int						width;
	int						height;
	int						tileWidth;
	int						tileHeight;

	SString				name;
	Point<float>			startingPosition;

	SDL_Color				backgroundColor;
	MapTypes				type;
	List<TileSet*>		tilesets;
	List<MapLayer*>		layers;
	List<ObjectGroup*>	objectGroups;
};

// ----------------------------------------------------
class Map : public Module
{
public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();
	void DrawAnimation(SString name, SString tileset, bool flip = false);
	void DrawStaticAnimation(SString name, SString tileset, iPoint position, AnimationInfo* animInfo);
	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tilesetNode, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tilesetNode, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadObjectLayers(pugi::xml_node& node, ObjectGroup* group);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	bool LoadTilesetAnimation(pugi::xml_node& tilesetNode, TileSet* set);



	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

	float parallaxVelocity;

private:

	int i = 0;

	int frameCount = 1;

	pugi::xml_document	mapFile;
	SString			folder;
	bool				mapLoaded;

	float parallax, normalSpeed, secondParallax;

	SString prevAnimName = "idle";
	SString prevSAnimName;
};

#endif // __MAP_H__