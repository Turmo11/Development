#ifndef __MAP_H__
#define __MAP_H__

#include "External/PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "p2SString.h"
#include "Module.h"

struct AnimationInfo;
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
	ENEMY,
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

struct Animations
{
	p2SString			name = "idle";
	uint				id;				//Tile which is animated
	uint				n_frames;	//Number of frames of the animation
	uint*				frames;
	uint				speed;
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
	p2List<Animations*> animations;

	SDL_Rect* player_tile_rect = new SDL_Rect;
	SDL_Rect* PlayerTileRect(uint tile_id) {

		SDL_Rect* ret = player_tile_rect;

		int num_t_width = tex_width / tile_width;
		int num_t_height = tex_height / tile_height;

		int x = tile_id % num_t_width;
		int y = tile_id / num_t_width;


		ret->x = x * tile_width;
		ret->y = y * tile_height;
		ret->w = tile_width;
		ret->h = tile_height;

		return ret;
	}

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

	p2SString				name;
	p2Point<float>			starting_position;

	SDL_Color				background_color;
	map_types				type;
	p2List<TileSet*>		tilesets;
	p2List<MapLayer*>		layers;
	p2List<ObjectGroup*>	object_groups;
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
	void DrawAnimation(p2SString name, p2SString tileset, bool flip = false);
	void DrawStaticAnimation(p2SString name, p2SString tileset, iPoint position, AnimationInfo* anim_info);
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
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadObjectLayers(pugi::xml_node& node, ObjectGroup* group);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	bool LoadTilesetAnimation(pugi::xml_node& tileset_node, TileSet* set);



	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

	float parallax_velocity = 0.05f;

private:

	int i = 0;

	int frame_count = 1;

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;

	float parallax, normal_speed, second_parallax, third_parallax;

	p2SString prev_anim_name = "idle";
	p2SString prev_s_anim_name;
};

#endif // __MAP_H__