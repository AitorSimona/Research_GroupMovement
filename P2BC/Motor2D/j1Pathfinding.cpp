#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "Brofiler/Brofiler.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.create("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t >= 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if (App->scene->firstStage == true)
	{
		if (CheckBoundaries(pos) && App->map->data.layers.end->data->Get(pos.x, pos.y) < App->map->data.tilesets.start->next->next->data->firstgid)
			return map[(pos.y*width) + pos.x];
	}
	else
	{
		if (CheckBoundaries(pos) && App->map->data2.layers.end->data->Get(pos.x, pos.y) < App->map->data2.tilesets.start->next->next->data->firstgid)
			return map[(pos.y*width) + pos.x];
	}


	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const p2DynArray<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.Score() < min)
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

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent, uint diagonal) : g(g), h(h), pos(pos), parent(parent), diagonal(diagonal)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.count();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, 0));

	// north east
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, 1));

	// north west
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, 1));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, 0));


	// south east
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, 1));

	// south west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, 1));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, 0));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, 0));

	return list_to_fill.list.count();
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
	if (this->diagonal == 0)
		g = parent->g + 1;
	else
		g = parent->g + 2;

	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	BROFILER_CATEGORY("Pathfinding_CreatePath", Profiler::Color::DarkOliveGreen);

	// TODO 1: if origin or destination are not walkable, return -1

	int ret = 1;

	// --- We ensure that origin nor destination are not walkable tiles and that they are not the same ---
	if (IsWalkable(origin) == false
		|| IsWalkable(destination) == false
		|| origin == destination)
	{
		ret = -1;
		return ret;
	}

	// --- We declare the lists needed to create the path ---
	PathList open;
	PathList closed;
	PathList adjacent;

	// --- We add the origin to the open/frontier list ---
	open.list.add(PathNode(0, 0, origin, NULL, 0));


	// --- We iterate until no element is left in frontier/open list ---
	while (open.list.count() > 0)
	{
		// --- We delete the lowest score tile from open and move it to closed, frontier-->visited ---
		p2List_item<PathNode>* lowest = open.GetNodeLowestScore();
		p2List_item<PathNode>* node = closed.list.add(lowest->data);
		open.list.del(lowest);


		// --- If we find the destination we do backtracking and create the path, breaking the loop ---
		if (node->data.pos == destination)
		{
			last_path.Clear();
			// Backtrack to create the final path
			const PathNode* path_node = &node->data;

			while (path_node)
			{
				last_path.PushBack(path_node->pos);
				path_node = path_node->parent;
			}

			last_path.Flip();
			break;
		}

		// --- If we have not found the destination we proceed to find the current tile's neighbors ---

		// Fill a list with all adjacent nodes
		adjacent.list.clear();
		node->data.FindWalkableAdjacents(adjacent);

		p2List_item<PathNode>* item = adjacent.list.start;

		// --- While we have neighbors left to check ... iterate! ---
		for (; item; item = item->next)
		{
			//If we find it in the closed list we do not want to continue
			if (closed.Find(item->data.pos) == NULL)
			{

				p2List_item <PathNode>* adjacent_in_open = open.Find(item->data.pos);

				// --- If we do not find the neighbor in the open/frontier list we calculate its cost (F) and add it to the list ---
				if (adjacent_in_open == NULL)
				{
					item->data.CalculateF(destination);
					open.list.add(item->data);
				}

				// --- If we have found it in open/frontier list ... ---
				else
				{
					// --- We compare walkable cost and if the neighbor's is bigger than current + offset we know this is a better path and update neighbor's parent and F ---
					/*if (adjacent_in_open->data.g > item->data.g + 1)
					{
					adjacent_in_open->data.parent = item->data.parent;
					adjacent_in_open->data.CalculateF(destination);
					}*/

					if (adjacent_in_open->data.diagonal == false)
					{
						if (adjacent_in_open->data.g > item->data.g + 20)
						{
							adjacent_in_open->data.parent = item->data.parent;
							adjacent_in_open->data.CalculateF(destination);
						}
					}
					else
					{
						if (adjacent_in_open->data.g > item->data.g + 30)
						{
							adjacent_in_open->data.parent = item->data.parent;
							adjacent_in_open->data.CalculateF(destination);
						}
					}
				}
			}
		}

	}

	return ret;
}