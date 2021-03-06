#include "App.h"
#include "PathFinding.h"

#include "Defs.h"
#include "Log.h"

PathFinding::PathFinding() : Module(), map(NULL), lastPath(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.Create("pathfinding");
}

// Destructor
PathFinding::~PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	lastPath.Clear();
	RELEASE_ARRAY(map);

	return true;
}
void PathFinding::ResetPath(iPoint start)
{
	frontier.Clear();
	visited.Clear();
	breadcrumbs.Clear();

	frontier.Push(start, 0);
	visited.Add(start);
	breadcrumbs.Add(start);
}
// Sets up the walkability map
void PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width * height];
	memcpy(map, data, width * height);
}
// To request all tiles involved in the last generated path
DynArray<iPoint>* PathFinding::GetLastPath()
{
	return &lastPath;
}
PQueue<iPoint>* PathFinding::GetFrontier()
{
	return &frontier;
}
List<iPoint>* PathFinding::GetVisited()
{
	return &visited;
}
List<iPoint>* PathFinding::GetBreadcrumbs()
{
	return &breadcrumbs;
}
// Utility: return true if pos is inside the map boundaries
bool PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}
// Utility: returns true is the tile is walkable
bool PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}
// Utility: return the walkability value of a tile
uchar PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y * width) + pos.x];

	return INVALID_WALK_CODE;
}
//int PathFinding::MovementCost(iPoint pos) const
//{
//	int ret = -1;
//
//	if (CheckBoundaries(pos))
//	{
//		int id = app->map->data.layers.start->next->data->Get(pos.x, pos.y);
//
//		if (id == 0) ret = 3;
//		else ret = 0;
//	}
//
//	return ret;
//}
void PathFinding::PropagateAStar(const iPoint& destination)
{
	// L12a: TODO 2: Implement AStar algorythm
	// Consider the different heuristics
	iPoint curr;
	int newCost[4];
	if (frontier.Pop(curr))
	{
		iPoint neighbors[4];
		neighbors[0].Create(curr.x + 0, curr.y + 1);
		neighbors[1].Create(curr.x - 1, curr.y + 0);
		neighbors[2].Create(curr.x + 1, curr.y + 0);
		neighbors[3].Create(curr.x + 0, curr.y - 1);

		int j = 0;
		bool init = false;
		for (uint i = 0; i < 4; ++i)
		{
			if (GetTileAt(neighbors[i]) > 0)
			{
				if (visited.Find(neighbors[i]) == -1)
				{
					//visited.Add(neighbors[i]);
					newCost[i] = neighbors[i].DistanceManhattan(visited.start->data) + neighbors[i].DistanceManhattan(destination);
					if (newCost[i] >= 0 && init == false)j = i, init = true;
				}
			}
		}

		for (int i = 0; i < 3; i++)
		{
			if (visited.Find(neighbors[i]) == -1)
			{
				if (GetTileAt(neighbors[i]) > 0 && newCost[i + 1] >= 0 && newCost[j] >= newCost[i + 1])j = i + 1;
				//if (!(MovementCost(neighbors[i].x, neighbors[i].y) > 0) && i == 0)j++;
			}
		}
		for (int i = 0; i < 4; i++)
		{
			if (visited.Find(neighbors[i]) == -1 && GetTileAt(neighbors[i]) > 0)
			{
				visited.Add(neighbors[i]);
				breadcrumbs.Add(curr);
				if (newCost[j] == newCost[i])
				{
					frontier.Push(neighbors[i], newCost[i]);
				}
			}
		}
	}
}
void PathFinding::ComputePathAStar(const iPoint& origin, const iPoint& destination)
{
	//ResetPath(origin);
	int count = 0;
	while (destinationIsFind == false)
	{
		PropagateAStar(destination);
		for (count; count < visited.Count(); count++)
		{
			if (visited.At(count)->data.x == destination.x && visited.At(count)->data.y == destination.y)
			{
				destinationIsFind = true;
				break;
			}
		}
	}
	destinationIsFind = false;
	//PropagateAStar(destination);
	lastPath.Clear();

	lastPath.PushBack(destination);
	lastPath.PushBack(breadcrumbs.At(count)->data);
	for (int i = visited.Count() - 1; i > 0; i--)
	{
		if (visited.At(i)->data == breadcrumbs.At(count)->data)
		{
			lastPath.PushBack(breadcrumbs.At(i)->data);
			count = i;
		}
	}
 	lastPath.Flip();
}
