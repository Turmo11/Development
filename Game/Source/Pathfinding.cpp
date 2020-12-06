#include "Defs.h"
#include "Log.h"
#include "Application.h"
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
	map = new uchar[width * height];
	memcpy(map, data, width * height);
}

// Utility: return true if pos is inside the map boundaries
bool Pathfinding::CheckBoundaries(const iPoint& pos) const
{
	bool debug = (pos.x >= 0 && pos.x <= (int)width && pos.y >= 0 && pos.y <= (int)height);
	return debug;
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
	if (CheckBoundaries(pos))
	{
		return map[(pos.y * width) + pos.x];
	}
	return INVALID_WALK_CODE;
}

PathMovement Pathfinding::CheckDirection(DynArray<iPoint>& path, PathMovement lastMov) const
{
	if(path.Count() != 0)
	{
		if (path.Count() >= 2)
		{
			iPoint tile = path[0];
			iPoint nextTile = path[1];

			int xDiff = nextTile.x - tile.x;
			int yDiff = nextTile.y - tile.y;

			if (xDiff == 1 && yDiff == 1) return PathMovement::DOWN_RIGHT;
			else if (xDiff == 1 && yDiff == -1) return PathMovement::UP_RIGHT;
			else if (xDiff == -1 && yDiff == 1) return PathMovement::DOWN_LEFT;
			else if (xDiff == -1 && yDiff == -1) return PathMovement::UP_LEFT;
			else if (xDiff == 1) return PathMovement::RIGHT;
			else if (xDiff == -1) return PathMovement::LEFT;
			else if (yDiff == 1) return PathMovement::DOWN;
			else if (yDiff == -1) return PathMovement::UP;
		}
		else if (path.Count() == 1)
		{
			return lastMov;
		}
		else
		{
			return PathMovement::NO_MOVE;
		}
	}
	
}

PathMovement Pathfinding::CheckDirectionGround(DynArray<iPoint>& path) const
{
	if (path.Count() >= 2)
	{
		iPoint tile = path[0];
		iPoint nextTile = path[1];

		int xDiff = nextTile.x - tile.x;
		int yDiff = nextTile.y - tile.y;

		if (xDiff == 1) return PathMovement::RIGHT;
		else if (xDiff == -1) return PathMovement::LEFT;
		else if (yDiff == 1)	return PathMovement::DOWN;
		else if (yDiff == -1) return PathMovement::UP;
	}
	else return PathMovement::NO_MOVE;
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

	// UP RIGHT
	cell.create(pos.x + 1, pos.y + 1);
	if (app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// UP LEFT
	cell.create(pos.x - 1, pos.y + 1);
	if (app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// DOWN RIGHT
	cell.create(pos.x + 1, pos.y - 1);
	if (app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// DOWN LEFT
	cell.create(pos.x - 1, pos.y - 1);
	if (app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// UP
	cell.create(pos.x, pos.y + 1);
	if (app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// DOWN
	cell.create(pos.x, pos.y - 1);
	if (app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// RIGHT
	cell.create(pos.x + 1, pos.y);
	if (app->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// LEFT
	cell.create(pos.x - 1, pos.y);
	if (app->pathfinding->IsWalkable(cell))
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
	/*g = parent->g + 1;

	int xDist = abs(pos.x - destination.x);
	int yDist = abs(pos.y - destination.y);

	h = (xDist + yDist) * min(xDist, yDist);*/

	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
DynArray<iPoint>* Pathfinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	//BROFILER_CATEGORY("Pathfinding::CreatePath", Profiler::Color::Chartreuse);
	lastPath.Clear();

	// Making sure origin and destination are walkable
	if (IsWalkable(origin) && IsWalkable(destination))
	{
		// Creating list open and close
		PathList open, close;
		PathNode origin(0, origin.DistanceNoSqrt(destination), origin, nullptr);
		open.list.add(origin);

		while (open.list.count() > 0)
		{
			close.list.add(open.GetNodeLowestScore()->data);
			open.list.del(open.GetNodeLowestScore());

			if (close.list.end->data.pos != destination) {

				PathList adjacent;

				close.list.end->data.FindWalkableAdjacents(adjacent);

				for (ListItem<PathNode>* iterator = adjacent.list.start; iterator != nullptr; iterator = iterator->next)
				{
					if (close.Find(iterator->data.pos))
						continue;

					else if (open.Find(iterator->data.pos))
					{
						PathNode tmp = open.Find(iterator->data.pos)->data;
						iterator->data.CalculateF(destination);

						if (tmp.g > iterator->data.g)
						{
							tmp.parent = iterator->data.parent;
						}
					}

					else
					{
						iterator->data.CalculateF(destination);
						open.list.add(iterator->data);
					}
				}
				adjacent.list.clear();
			}

			else
			{
				for (ListItem<PathNode>* iterator = close.list.end; iterator->data.parent != nullptr; iterator = close.Find(iterator->data.parent->pos))
				{
					lastPath.PushBack(iterator->data.pos);

					if (iterator->data.parent == nullptr)
						lastPath.PushBack(close.list.start->data.pos);
				}

				lastPath.Flip();

				return &lastPath;
			}
		}
	}

	else return nullptr;
}

