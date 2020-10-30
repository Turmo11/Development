#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "EntityPlayer.h"
#include "Pickups.h"
#include <math.h>

Map::Map() : Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	parallax = config.child("parallax").attribute("value").as_float();
	second_parallax = config.child("second_parallax").attribute("value").as_float();
	third_parallax = config.child("third_parallax").attribute("value").as_float();
	normal_speed = config.child("normal_speed").attribute("value").as_float();

	return ret;
}

bool Map::Save(pugi::xml_node& node) const
{
	LOG("Saving Map...");
	node.append_child("map_name").append_attribute("filename") = data.name.GetString();

	return true;
}

bool Map::Load(pugi::xml_node& node)
{
	LOG("Loading Map...");

	Load(node.child("map_name").attribute("filename").as_string());


	return true;
}

void Map::Draw()
{
	if (map_loaded == false)
		return;

	// TODO 4: Make sure we draw all the layers and not just the first one
	p2List_item<MapLayer*>* layer = data.layers.start;
	while (layer != nullptr) {
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				if (layer->data->name == "Parallax")
					parallax_velocity = parallax;
					
				else if (layer->data->name == "SecondParallax")
					parallax_velocity = second_parallax;
				else
					parallax_velocity = normal_speed;

				int tile_id = layer->data->Get(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					if (tileset != nullptr)
					{
						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);

						App->render->Blit(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE, parallax_velocity);
					}
				}
			}
		}
		layer = layer->next;
	}
}

void Map::DrawAnimation(p2SString name, p2SString tileset, bool flip)
{

	TileSet* anim_tileset = nullptr;

	p2List_item<TileSet*>* tileset_iterator = data.tilesets.start;

	while (tileset_iterator != NULL)
	{
		if (tileset_iterator->data->name == tileset)
		{
			anim_tileset = tileset_iterator->data;
		}
		tileset_iterator = tileset_iterator->next;
	}

	// I have the adventurer Tileset inside I have animation
	Animations* current_anim = nullptr;

	p2List_item<Animations*>* anim_iterator;
	anim_iterator = anim_tileset->animations.start;

	while (anim_iterator)
	{
		if (name == anim_iterator->data->name)
		{
			current_anim = anim_iterator->data; //gets the animation with the name we sent
		}
		anim_iterator = anim_iterator->next;
	}

	if (prev_anim_name != current_anim->name) // So that when animations change they start from frame 0
	{
		i = 0;
		frame_count = 1;
	}

	prev_anim_name = current_anim->name;

	App->render->Blit(anim_tileset->texture,									//Texture of the animation(tileset) 
	App->player->player.position.x, App->player->player.position.y,			//drawn at player position
	anim_tileset->PlayerTileRect(current_anim->frames[i]), flip);			//draw frames tile id

	if (frame_count % (current_anim->speed / 10) == 0)	//counts frames each loop (60 fps using vsync) Magic Numbers
	{
		i++;
	}
	if (i >= current_anim->n_frames)		//Iterate from 0 to n_frames (number of frames in animation)
	{				
		i = 0;
	}

	frame_count++;
}

void Map::DrawStaticAnimation(p2SString name, p2SString tileset, iPoint position, AnimationInfo* anim_info)
{

	TileSet* s_anim_tileset = nullptr;

	p2List_item<TileSet*>* tileset_iterator = data.tilesets.start;

	while (tileset_iterator != NULL)
	{
		if (tileset_iterator->data->name == tileset)
		{
			s_anim_tileset = tileset_iterator->data;
		}
		tileset_iterator = tileset_iterator->next;
	}

	Animations* current_anim = nullptr;

	p2List_item<Animations*>* anim_iterator;
	anim_iterator = s_anim_tileset->animations.start;

	while (anim_iterator)
	{
		if (name == anim_iterator->data->name)
		{
			current_anim = anim_iterator->data; //gets the animation with the name we sent
		}
		anim_iterator = anim_iterator->next;
	}

	if (anim_info->prev_s_anim_name != current_anim->name) // So that when animations change they start from frame 0
	{
		anim_info->i = 0;
		anim_info->frame_count = 1;
	}

	anim_info->prev_s_anim_name = current_anim->name;

	App->render->Blit(s_anim_tileset->texture, position.x - 16, position.y - 16, s_anim_tileset->PlayerTileRect(current_anim->frames[anim_info->i]));			

	if (anim_info->frame_count > current_anim->speed / 10)	//counts time for each frame of animation
	{
		anim_info->i++;
		anim_info->frame_count = 1;
	}

	if (anim_info->i >= current_anim->n_frames)  //Iterate from 0 to nFrames (number of frames in animation)
	{
		anim_info->i = 0;
	}

	anim_info->frame_count++;
}


TileSet* Map::GetTilesetFromTileId(int id) const
{
	// TODO 3: Complete this method so we pick the right
	// Tileset based on a tile id //data.tilesets.start->data
	TileSet* ret = nullptr;

	p2List_item<TileSet*>* i = data.tilesets.start;
	while (i->next != nullptr) {

		if (id >= i->data->firstgid && id < i->next->data->firstgid) {
			ret = i->data;
			break;
		}
		i = i->next;
	}
	if (ret == nullptr) {
		ret = data.tilesets.end->data;
	}

	return ret;
}

iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if (data.type == MapTypes::ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MapTypes::ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MapTypes::ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MapTypes::ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2);
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while (item != NULL)
	{
		item->data->animations.clear();
		delete item->data->player_tile_rect;
		

		SDL_DestroyTexture(item->data->texture);

		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	//Object cleanup
	p2List_item<ObjectGroup*>* item3;
	item3 = App->map->data.object_groups.start;

	while (item3 != NULL)
	{
		if (item3->data->objects != NULL)
			delete[] item3->data->objects;

		delete item3->data;

		item3 = item3->next;
	}

	App->map->data.object_groups.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	p2SString* fileName = new p2SString(file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		data.name = fileName->GetString();
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetAnimation(tileset, set);
		}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* map_layer = new MapLayer();

		ret = LoadLayer(layer, map_layer);

		if (ret == true)
			data.layers.add(map_layer);
	}

	// Load Collider info
	pugi::xml_node object_group;

	for (object_group = map_file.child("map").child("objectgroup"); object_group && ret; object_group = object_group.next_sibling("objectgroup"))
	{
		ObjectGroup* new_object_group = new ObjectGroup();

		if (ret == true)
		{
			ret = LoadObjectLayers(object_group, new_object_group);
		}

		data.object_groups.add(new_object_group);
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while (item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while (item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.starting_position.x = map.child("properties").child("property").attribute("value").as_float();
		data.starting_position.y = map.child("properties").child("property").next_sibling().attribute("value").as_float();

		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if (bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if (orientation == "orthogonal")
		{
			data.type = MapTypes::ORTHOGONAL;
		}
		else if (orientation == "isometric")
		{
			data.type = MapTypes::ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = MapTypes::STAGGERED;
		}
		else
		{
			data.type = MapTypes::UNKNOWN;
		}
	}

	return ret;
}

// Load Tileset data
bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if (offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

//Load Tileset Image
bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

//Load different map layers
bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

//Load object layer for colliders
bool Map::LoadObjectLayers(pugi::xml_node& node, ObjectGroup* object_group)
{
	object_group->name = node.attribute("name").as_string();
	object_group->id = node.attribute("id").as_uint();

	int AmountObjects = 0;
	for (pugi::xml_node iterator_node = node.child("object"); iterator_node; iterator_node = iterator_node.next_sibling("object"), AmountObjects++) {}

	object_group->objects_size = AmountObjects;
	object_group->objects = new Object[AmountObjects];
	memset(object_group->objects, 0, AmountObjects * sizeof(Object));

	int i = 0;
	for (pugi::xml_node iterator_node = node.child("object"); iterator_node; iterator_node = iterator_node.next_sibling("object"), i++) {
		SDL_Rect* collider = new SDL_Rect;

		collider->x = iterator_node.attribute("x").as_uint();
		collider->y = iterator_node.attribute("y").as_uint();
		collider->w = iterator_node.attribute("width").as_uint();
		collider->h = iterator_node.attribute("height").as_uint();

		object_group->objects[i].collider = collider;
		object_group->objects[i].id = iterator_node.attribute("id").as_uint();
		object_group->objects[i].name = iterator_node.attribute("name").as_string();

		p2SString type(iterator_node.attribute("type").as_string());

		if (type == "ground")
		{
		object_group->objects[i].type = ObjectType::GROUND;
		}
		else if (type == "platform")
		{
			object_group->objects[i].type = ObjectType::PLATFORM;
		}
		else if (type == "letter")
		{
			object_group->objects[i].type = ObjectType::LETTER;

			Properties::Property* temp = new Properties::Property;
			temp->name = iterator_node.child("properties").child("property").attribute("name").as_string();
			temp->data.v_string = iterator_node.child("properties").child("property").attribute("value").as_string();
			object_group->objects[i].properties.property_list.add(temp);
		}
		else if (type == "enemy")
		{
			object_group->objects[i].type = ObjectType::ENEMY;
		}
		else if (type == "goal")
		{
			object_group->objects[i].type = ObjectType::GOAL;

			Properties::Property* temp = new Properties::Property;
			temp->name = iterator_node.child("properties").child("property").attribute("name").as_string();
			temp->data.v_string = iterator_node.child("properties").child("property").attribute("value").as_string();
			object_group->objects[i].properties.property_list.add(temp);
		}
		else if (type == "death")
		{
			object_group->objects[i].type = ObjectType::DEATH;
			Properties::Property* temp = new Properties::Property;
			temp->name = iterator_node.child("properties").child("property").attribute("name").as_string();
			temp->data.v_string = iterator_node.child("properties").child("property").attribute("value").as_string();
			object_group->objects[i].properties.property_list.add(temp);
		}
		else
		{
			object_group->objects[i].type = ObjectType::UNKNOWN;
		}

	}

	return true;
}

// Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)							//REVISE THIS HERE. Check why it crashes the game at exit time.
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != nullptr)
	{
		pugi::xml_node property;

		for (property = data.child("property"); property; property = property.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = property.attribute("name").as_string();

			properties.property_list.add(p);
		}
	}

	return ret;
}

// Returns a property
value Properties::Get(const char* name, value* default_value) const
{
	p2List_item<Property*>* item = property_list.start;

	while (item)
	{
		if (item->data->name == name)
			return item->data->data;
		item = item->next;
	}

	return *default_value;
}

// Load Animations from tileset
bool Map::LoadTilesetAnimation(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;

	for (pugi::xml_node iterator_node = tileset_node.child("tile"); iterator_node; iterator_node = iterator_node.next_sibling("tile")) { //Iterator for all animation childs

		Animations* new_animation = new Animations;

		new_animation->id = iterator_node.attribute("id").as_uint(); // Get the id of the animated tile

		new_animation->name = iterator_node.child("properties").child("property").attribute("name").as_string(); //Get the name of the animation inside extra attribute

		new_animation->name = iterator_node.child("properties").child("property").attribute("value").as_string(); //Get the name of the animation inside extra attribute

		new_animation->frames = new uint[24]; // new array for frames

		memset(new_animation->frames, 0, 24); // allocate the new array

		int j = 0;
		for (pugi::xml_node iterator_node_anim = iterator_node.child("animation").child("frame"); iterator_node_anim; j++) { //Enters the frame of the animation child inside the tile we are in

			new_animation->frames[j] = iterator_node_anim.attribute("tileid").as_uint(); //Set frames ids
			new_animation->speed = iterator_node_anim.attribute("duration").as_uint();//set animation speed
			iterator_node_anim = iterator_node_anim.next_sibling("frame"); // next frame
		}

		new_animation->n_frames = j;

		set->animations.add(new_animation);
	}

	return ret;
};
