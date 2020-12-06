#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "EntityPlayer.h"
#include "Pickups.h"
#include "Pathfinding.h"
#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
	name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.Create(config.child("folder").child_value());

	parallax = config.child("parallax").attribute("value").as_float();
	secondParallax = config.child("secondParallax").attribute("value").as_float();
	normalSpeed = config.child("normalSpeed").attribute("value").as_float();

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
	if (mapLoaded == false)
		return;

	// TODO 4: Make sure we draw all the layers and not just the first one
	ListItem<MapLayer*>* layer = data.layers.start;
	while (layer != nullptr) {

		if (layer->data->navigation == true && debug == false) //Render walkability map only when debugging
		{ 
			break; 
		}
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				if (layer->data->name == "Parallax")
					parallaxVelocity = parallax;

				else if (layer->data->name == "SecondParallax")
					parallaxVelocity = secondParallax;
				else
					parallaxVelocity = normalSpeed;

				int tileId = layer->data->Get(x, y);
				if (tileId > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tileId);
					if (tileset != nullptr)
					{
						SDL_Rect r = tileset->GetTileRect(tileId);
						iPoint pos = MapToWorld(x, y);

						app->render->DrawTexture(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE, parallaxVelocity);
					}
				}
			}
		}
		layer = layer->next;
	}
}

void Map::DrawAnimation(SString name, SString tileset, bool flip)
{

	TileSet* animTileset = nullptr;

	ListItem<TileSet*>* tilesetIterator = data.tilesets.start;

	while (tilesetIterator != NULL)
	{
		if (tilesetIterator->data->name == tileset)
		{
			animTileset = tilesetIterator->data;
		}
		tilesetIterator = tilesetIterator->next;
	}

	// I have the adventurer Tileset inside I have animation
	Animations* currentAnim = nullptr;

	ListItem<Animations*>* animIterator;
	animIterator = animTileset->animations.start;

	while (animIterator)
	{
		if (name == animIterator->data->name)
		{
			currentAnim = animIterator->data; //gets the animation with the name we sent
		}
		animIterator = animIterator->next;
	}

	if (prevAnimName != currentAnim->name) // So that when animations change they start from frame 0
	{
		i = 0;
		frameCount = 1;
	}

	prevAnimName = currentAnim->name;

	app->render->DrawTexture(animTileset->texture,									//Texture of the animation(tileset) 
		app->player->player.position.x, app->player->player.position.y,			//drawn at player position
		animTileset->PlayerTileRect(currentAnim->frames[i]), flip);			//draw frames tile id

	if (frameCount % (currentAnim->speed / 10) == 0)	//counts frames each loop (60 fps using vsync) Magic Numbers
	{
		i++;
	}
	if (i >= currentAnim->nFrames)		//Iterate from 0 to nFrames (number of frames in animation)
	{
		i = 0;
	}

	frameCount++;
}

void Map::DrawStaticAnimation(SString name, SString tileset, iPoint position, AnimationInfo* animInfo)
{

	TileSet* sAnimTileset = nullptr;

	ListItem<TileSet*>* tilesetIterator = data.tilesets.start;

	while (tilesetIterator != NULL)
	{
		if (tilesetIterator->data->name == tileset)
		{
			sAnimTileset = tilesetIterator->data;
		}
		tilesetIterator = tilesetIterator->next;
	}

	Animations* currentAnim = nullptr;

	ListItem<Animations*>* animIterator;
	animIterator = sAnimTileset->animations.start;

	while (animIterator)
	{
		if (name == animIterator->data->name)
		{
			currentAnim = animIterator->data; //gets the animation with the name we sent
		}
		animIterator = animIterator->next;
	}

	if (animInfo->prevSAnimName != currentAnim->name) // So that when animations change they start from frame 0
	{
		animInfo->i = 0;
		animInfo->frameCount = 1;
	}

	animInfo->prevSAnimName = currentAnim->name;

	app->render->DrawTexture(sAnimTileset->texture, position.x - 16, position.y - 16, sAnimTileset->PlayerTileRect(currentAnim->frames[animInfo->i]), animInfo->flip);

	if (animInfo->frameCount > currentAnim->speed / 10)	//counts time for each frame of animation
	{
		animInfo->i++;
		animInfo->frameCount = 1;
	}

	if (animInfo->i >= currentAnim->nFrames)  //Iterate from 0 to nFrames (number of frames in animation)
	{
		animInfo->i = 0;
	}

	animInfo->frameCount++;
}


TileSet* Map::GetTilesetFromTileId(int id) const
{
	// TODO 3: Complete this method so we pick the right
	// Tileset based on a tile id //data.tilesets.start->data
	TileSet* ret = nullptr;

	ListItem<TileSet*>* i = data.tilesets.start;
	while (i->next != nullptr)
	{

		if (id >= i->data->firstgid && id < i->next->data->firstgid)
		{
			ret = i->data;
			break;
		}
		i = i->next;
	}
	if (ret == nullptr)
	{
		ret = data.tilesets.end->data;
	}

	return ret;
}

iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if (data.type == MapTypes::ORTHOGONAL)
	{
		ret.x = x * data.tileWidth;
		ret.y = y * data.tileHeight;
	}
	else if (data.type == MapTypes::ISOMETRIC)
	{
		ret.x = (x - y) * (data.tileWidth * 0.5f);
		ret.y = (x + y) * (data.tileHeight * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint Map::MapToWorldCentered(int x, int y) const
{
	iPoint ret;

	if (data.type == MapTypes::ORTHOGONAL)
	{
		ret.x = x * data.tileWidth + data.tileWidth / 2;
		ret.y = y * data.tileHeight + data.tileHeight / 2;
	}
	else if (data.type == MapTypes::ISOMETRIC)
	{
		ret.x = (x - y) * (data.tileWidth * 0.5f);
		ret.y = (x + y) * (data.tileHeight * 0.5f);
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
		ret.x = x / data.tileWidth;
		ret.y = y / data.tileHeight;
	}

	else if (data.type == MapTypes::ISOMETRIC)
	{
		float halfWidth = data.tileWidth * 0.5f;
		float halfHeight = data.tileHeight * 0.5f;
		ret.x = int((x / halfWidth + y / halfHeight) / 2);
		ret.y = int((y / halfHeight - (x / halfWidth)) / 2);
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
	int relativeId = id - firstgid;
	SDL_Rect rect;
	rect.w = tileWidth;
	rect.h = tileHeight;
	rect.x = margin + ((rect.w + spacing) * (relativeId % numTilesWidth));
	rect.y = margin + ((rect.h + spacing) * (relativeId / numTilesWidth));

	return rect;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	ListItem<TileSet*>* item;
	item = data.tilesets.start;

	while (item != NULL)
	{
		item->data->animations.clear();
		delete item->data->playerTileRect;


		SDL_DestroyTexture(item->data->texture);

		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	ListItem<MapLayer*>* item2;
	item2 = data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	//Object cleanup
	ListItem<ObjectGroup*>* item3;
	item3 = app->map->data.objectGroups.start;

	while (item3 != NULL)
	{
		if (item3->data->objects != NULL)
			delete[] item3->data->objects;

		delete item3->data;

		item3 = item3->next;
	}

	app->map->data.objectGroups.clear();

	// Clean up the pugui tree
	mapFile.reset();

	return true;
}

// Load new map
bool Map::Load(const char* fileCName)
{
	bool ret = true;
	SString tmp("%s%s", folder.GetString(), fileCName);

	SString* fileName = new SString(fileCName);

	pugi::xml_parse_result result = mapFile.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", fileCName, result.description());
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
	for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
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
	for (layer = mapFile.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* mapLayers = new MapLayer();

		ret = LoadLayer(layer, mapLayers);

		if (ret == true)
			data.layers.add(mapLayers);
	}

	// Load Collider info
	pugi::xml_node objectGroup;

	for (objectGroup = mapFile.child("map").child("objectgroup"); objectGroup && ret; objectGroup = objectGroup.next_sibling("objectgroup"))
	{
		ObjectGroup* newObjectGroup = new ObjectGroup();

		if (ret == true)
		{
			ret = LoadObjectLayers(objectGroup, newObjectGroup);
		}

		data.objectGroups.add(newObjectGroup);
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", fileCName);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tileWidth: %d tileHeight: %d", data.tileWidth, data.tileHeight);

		ListItem<TileSet*>* item = data.tilesets.start;
		while (item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tileWidth, s->tileHeight);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		ListItem<MapLayer*>* itemLayer = data.layers.start;
		while (itemLayer != NULL)
		{
			MapLayer* l = itemLayer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			itemLayer = itemLayer->next;
		}
	}
	//Walkability Map
	int w, h;
	uchar* data = NULL;
	if (app->map->CreateWalkabilityMap(w, h, &data))
	{
		app->pathfinding->SetMap(w, h, data);
	}
		
	mapLoaded = ret;

	return ret;
}

// Load map general properties
bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = mapFile.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.startingPosition.x = map.child("properties").child("property").attribute("value").as_float();
		data.startingPosition.y = map.child("properties").child("property").next_sibling().attribute("value").as_float();

		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tileWidth = map.attribute("tilewidth").as_int();
		data.tileHeight = map.attribute("tileheight").as_int();
		SString bgColor(map.attribute("backgroundcolor").as_string());

		data.backgroundColor.r = 0;
		data.backgroundColor.g = 0;
		data.backgroundColor.b = 0;
		data.backgroundColor.a = 0;

		if (bgColor.Length() > 0)
		{
			SString red, green, blue;
			bgColor.SubString(1, 2, red);
			bgColor.SubString(3, 4, green);
			bgColor.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.backgroundColor.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.backgroundColor.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.backgroundColor.b = v;
		}

		SString orientation(map.attribute("orientation").as_string());

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
bool Map::LoadTilesetDetails(pugi::xml_node& tilesetNode, TileSet* set)
{
	bool ret = true;
	set->name.Create(tilesetNode.attribute("name").as_string());
	set->firstgid = tilesetNode.attribute("firstgid").as_int();
	set->tileWidth = tilesetNode.attribute("tilewidth").as_int();
	set->tileHeight = tilesetNode.attribute("tileheight").as_int();
	set->margin = tilesetNode.attribute("margin").as_int();
	set->spacing = tilesetNode.attribute("spacing").as_int();
	pugi::xml_node offset = tilesetNode.child("tileoffset");

	if (offset != NULL)
	{
		set->offsetX = offset.attribute("x").as_int();
		set->offsetY = offset.attribute("y").as_int();
	}
	else
	{
		set->offsetX = 0;
		set->offsetY = 0;
	}

	return ret;
}

//Load Tileset Image
bool Map::LoadTilesetImage(pugi::xml_node& tilesetNode, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tilesetNode.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = app->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->texWidth = image.attribute("width").as_int();

		if (set->texWidth <= 0)
		{
			set->texWidth = w;
		}

		set->texHeight = image.attribute("height").as_int();

		if (set->texHeight <= 0)
		{
			set->texHeight = h;
		}

		set->numTilesWidth = set->texWidth / set->tileWidth;
		set->numTilesHeight = set->texHeight / set->tileHeight;
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
	if (strcmp(node.attribute("name").as_string(), "Navigation") == 0)
	{
		layer->navigation = node.child("properties").child("property").attribute("value").as_bool();
	}
	else
	{
		layer->navigation = false;
	}

	LoadProperties(node, layer->properties);
	pugi::xml_node layerData = node.child("data");

	if (layerData == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width * layer->height];
		memset(layer->data, 0, layer->width * layer->height);

		int i = 0;
		for (pugi::xml_node tile = layerData.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

//Load object layer for colliders
bool Map::LoadObjectLayers(pugi::xml_node& node, ObjectGroup* objectGroup)
{
	objectGroup->name = node.attribute("name").as_string();
	objectGroup->id = node.attribute("id").as_uint();

	int amountObjects = 0;
	for (pugi::xml_node iteratorNode = node.child("object"); iteratorNode; iteratorNode = iteratorNode.next_sibling("object"), amountObjects++) {}

	objectGroup->objectsSize = amountObjects;
	objectGroup->objects = new Object[amountObjects];
	memset(objectGroup->objects, 0, amountObjects * sizeof(Object));

	int i = 0;
	for (pugi::xml_node iteratorNode = node.child("object"); iteratorNode; iteratorNode = iteratorNode.next_sibling("object"), i++) {
		SDL_Rect* collider = new SDL_Rect;

		collider->x = iteratorNode.attribute("x").as_uint();
		collider->y = iteratorNode.attribute("y").as_uint();
		collider->w = iteratorNode.attribute("width").as_uint();
		collider->h = iteratorNode.attribute("height").as_uint();

		objectGroup->objects[i].collider = collider;
		objectGroup->objects[i].id = iteratorNode.attribute("id").as_uint();
		objectGroup->objects[i].name = iteratorNode.attribute("name").as_string();

		SString type(iteratorNode.attribute("type").as_string());

		if (type == "ground")
		{
			objectGroup->objects[i].type = ObjectType::GROUND;
		}
		else if (type == "platform")
		{
			objectGroup->objects[i].type = ObjectType::PLATFORM;
		}
		else if (type == "letter")
		{
			objectGroup->objects[i].type = ObjectType::LETTER;

			Properties::Property* temp = new Properties::Property;
			temp->name = iteratorNode.child("properties").child("property").attribute("name").as_string();
			temp->data.vString = iteratorNode.child("properties").child("property").attribute("value").as_string();
			objectGroup->objects[i].properties.propertyList.add(temp);
		}
		else if (type == "enemy")
		{
			objectGroup->objects[i].type = ObjectType::ENEMY;
		}
		else if (type == "goal")
		{
			objectGroup->objects[i].type = ObjectType::GOAL;

			Properties::Property* temp = new Properties::Property;
			temp->name = iteratorNode.child("properties").child("property").attribute("name").as_string();
			temp->data.vString = iteratorNode.child("properties").child("property").attribute("value").as_string();
			objectGroup->objects[i].properties.propertyList.add(temp);
		}
		else if (type == "death")
		{
			objectGroup->objects[i].type = ObjectType::DEATH;
			Properties::Property* temp = new Properties::Property;
			temp->name = iteratorNode.child("properties").child("property").attribute("name").as_string();
			temp->data.vString = iteratorNode.child("properties").child("property").attribute("value").as_string();
			objectGroup->objects[i].properties.propertyList.add(temp);
		}
		else if (type == "checkpoint")
		{
			objectGroup->objects[i].type = ObjectType::CHECKPOINT;
		}
		else
		{
			objectGroup->objects[i].type = ObjectType::UNKNOWN;
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

			properties.propertyList.add(p);
		}
	}

	return ret;
}

// Returns a property
value Properties::Get(const char* name, value* defaultValue) const
{
	ListItem<Property*>* item = propertyList.start;

	while (item)
	{
		if (item->data->name == name)
			return item->data->data;
		item = item->next;
	}

	return *defaultValue;
}

// Load Animations from tileset
bool Map::LoadTilesetAnimation(pugi::xml_node& tilesetNode, TileSet* set)
{
	bool ret = true;

	for (pugi::xml_node iteratorNode = tilesetNode.child("tile"); iteratorNode; iteratorNode = iteratorNode.next_sibling("tile")) //Iterator for all animation childs
	{
		Animations* newAnimation = new Animations;

		newAnimation->id = iteratorNode.attribute("id").as_uint(); // Get the id of the animated tile

		newAnimation->name = iteratorNode.child("properties").child("property").attribute("name").as_string(); //Get the name of the animation inside extra attribute

		newAnimation->name = iteratorNode.child("properties").child("property").attribute("value").as_string(); //Get the name of the animation inside extra attribute

		newAnimation->frames = new uint[24]; // new array for frames

		memset(newAnimation->frames, 0, 24); // allocate the new array

		int j = 0;
		for (pugi::xml_node iteratorNodeAnim = iteratorNode.child("animation").child("frame"); iteratorNodeAnim; j++) //Enters the frame of the animation child inside the tile we are in
		{
			newAnimation->frames[j] = iteratorNodeAnim.attribute("tileid").as_uint(); //Set frames ids
			newAnimation->speed = iteratorNodeAnim.attribute("duration").as_uint();//set animation speed
			iteratorNodeAnim = iteratorNodeAnim.next_sibling("frame"); // next frame
		}

		newAnimation->nFrames = j;

		set->animations.add(newAnimation);
	}

	return ret;
};

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	ListItem<MapLayer*>* item;
	item = data.layers.start;

	for (item = data.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->navigation == false)
			continue;

		uchar* map = new uchar[layer->width * layer->height];
		memset(map, 1, layer->width * layer->height * sizeof(uchar));

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y * layer->width) + x;

				int tile_id = layer->GetPath(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}
