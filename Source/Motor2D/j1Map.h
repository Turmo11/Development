#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "Dependencies/PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "p2SString.h"
#include "j1Module.h"

// ----------------------------------------------------
union value {
	const char*		v_string;
	int				v_int;
	float			v_float;
};

struct Properties //Properties
{
	struct Property
	{
		p2SString	name;
		value		data;
		
	};

	Properties::~Properties()															//Deletes every property and frees all allocated memory.
	{
		p2List_item<Property*>* item;
		item = property_list.start;
		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}
		property_list.clear();												//Clears property_list by deleting all items in the list and freeing all allocated memory.
	}

	value Get(const char* name, value* default_value = nullptr) const;

	p2List<Property*>	property_list;
};


//Objects for collisions
enum class object_type
{
	UNKNOWN = 0,
	PLAYER,
	GROUND,
	PLATFORM,
	LETTER,
	GOAL,
	DEATH
};

struct Object
{
	uint				id;
	p2SString			name;
	object_type			type;
	SDL_Rect*			collider;
	SDL_Texture*		texture;
	Properties			properties;
};

struct ObjectGroup
{
	uint				id;
	p2SString			name;
	Object*				objects;
	uint				objects_size;
};

// ----------------------------------------------------
struct MapLayer
{
	p2SString	name;
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

	p2SString			name;
	int					firstgid;
	int					tile_width;
	int					tile_height;
	int					margin;
	int					spacing;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum class map_types
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
	int						tile_width;
	int						tile_height;

	p2Point<float>			starting_position;

	SDL_Color				background_color;
	map_types				type;
	p2List<TileSet*>		tilesets;
	p2List<MapLayer*>		layers;
	p2List<ObjectGroup*>	object_groups;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadObjectLayers(pugi::xml_node& node, ObjectGroup* group);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__