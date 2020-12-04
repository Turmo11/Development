#include "Defs.h"
#include "Log.h"
#include "application.h"
#include "Pathfinding.h"
//#include "External/BroFiler/Brofiler.h"

Pathfinding::Pathfinding() : Module(), map(NULL), lastPath(DEFAULT_PATH_LENGTH),width(0), height(0)
{
	name.Create("pathfinding");
}

// Destructor
Pathfinding::~Pathfinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool Pathfinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	lastPath.Clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void Pathfinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool Pathfinding::CheckBoundaries(const iPoint& pos) const
{
	bool debug = (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool Pathfinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	bool debug = (t != INVALID_WALK_CODE && t > 0);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar Pathfinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const DynArray<iPoint>* Pathfinding::GetLastPath() const
{
	return &lastPath;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
ListItem<PathNode>* PathList::Find(const iPoint& point) const
{
	ListItem<PathNode>* item = list.start;
	while(item)
	{
		if(item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
ListItem<PathNode>* PathList::GetNodeLowestScore() const
{
	ListItem<PathNode>* ret = NULL;
	int min = 65535;

	ListItem<PathNode>* item = list.end;
	while(item)
	{
		if(item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& listToFill) const
{
	iPoint cell;
	uint before = listToFill.list.count();

	// north
	cell.create(pos.x, pos.y + 1);
	if(app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if(app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if(app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if(app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	return listToFill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int Pathfinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	//BROFILER_CATEGORY("Pathfinding::CreatePath", Profiler::Color::Chartreuse);
	int ret = -1;

	if (!IsWalkable(origin) || !IsWalkable(destination))
	{
		ret = -1;
		return ret;
	}
	int iterations = 0;

	PathList open;
	PathList close;


	open.list.add(PathNode(0, 0, origin, nullptr));

	while (open.list.count() != 0) {


		//Move the lowest score cell from open list to the closed list
		ListItem<PathNode>* node = close.list.add(open.GetNodeLowestScore()->data);
		open.list.del(open.GetNodeLowestScore());

		//If we just added the destination, we are done!
		if (node->data.pos == destination) {
			lastPath.Clear();


			// Backtrack to create the final path
			const PathNode *path_node = &node->data;
			while (path_node) {

				lastPath.PushBack(path_node->pos);

				path_node = path_node->parent;
			}


			// Use the Pathnode::parent and Flip() the path when you are finish
			lastPath.Flip();
			ret = lastPath.Count();
			break;
		}

	
		// Fill a list of all adjancent nodes
		PathList neighbours;
		node->data.FindWalkableAdjacents(neighbours);


		// Iterate adjancent nodes:

		// If it is NOT found, calculate its F and add it to the open list
		// If it is already in the open list, check if it is a better path (compare G)
		// If it is a better path, Update the parent
		
		ListItem<PathNode>* item = neighbours.list.start;

		while(item != nullptr)  {

			// ignore nodes in the closed list
			if (close.Find(item->data.pos) != NULL) {
				
				item = item->next;
				continue;
			}

			item->data.CalculateF(destination);
			

			if (open.Find(item->data.pos) == NULL) {
				open.list.add(item->data);
			}
			else if (item->data.g < open.Find(item->data.pos)->data.g) {
					
				open.Find(item->data.pos)->data.parent = item->data.parent;
			}

			item = item->next;
			iterations++;
		}
	}
	ret = iterations;
	return ret;
}

