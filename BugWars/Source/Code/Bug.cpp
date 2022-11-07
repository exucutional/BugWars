#include "pch.h"
#include "Bug.h"
#include "Game.h"
#include <array>

IMPLEMENT_RTTI(Bug);

Bug::Bug()
{
	visible = true;
	disabled = false;
}

void Bug::OnUpdate(float dt)
{
	g_Game->UpdateObj(this);
}

template<typename Container>
static void searchBugIn(Container& c, const Bug* me, float* min_dist, Bug** target)
{
	for (auto& object : c)
	{
		if (object->GetRTTI() == Bug::s_RTTI)
		{
			if (object == me)
				continue;

			if (object->disabled)
				continue;

			if (object->id > me->id)
				continue; // Can't eat that

			float dist = me->position.Distance(object->position);
			if (dist == 0)	// Do not allow zero vectors
				continue;

			if (dist < *min_dist)
			{
				*min_dist = dist;
				*target = static_cast<Bug*>(object);
			}
		}
	}
}

BugBase* Bug::FindBugToEat() const
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), __FUNCTION__);
	Bug* target = nullptr;
	if (id == 0)
		return target;

	float min_dist = std::numeric_limits<float>::max();
	cell_dim_t x_cell = (cell_dim_t)position.x / MapCellSize;
	cell_dim_t y_cell = (cell_dim_t)position.y / MapCellSize;
	auto cell = g_Game->GetMapCell(x_cell, y_cell, false);
	int level = 1;
	int maxLevel = std::max(std::max(cell->x,g_Game->map_dim - cell->x), std::max(cell->y, g_Game->map_dim - cell->y));
	maxLevel = std::max(1, maxLevel);
	auto compare = [this](MapCell* c1, MapCell* c2) { return g_Game->GetDistToCell(position, c1) < g_Game->GetDistToCell(position, c2); };
	std::multiset<MapCell*, decltype(compare)> cells(compare);
	while (min_dist > (level - 1) * MapCellSize && level <= maxLevel)
	{
		if (level == 1)
			searchBugIn(cell->objects, this, &min_dist, &target);

		cells.clear();
		g_Game->GetMapCellNeighbours(cells, cell, level++, position);
		for (auto cell : cells)
			if (cell && g_Game->GetDistToCell(position, cell) < min_dist)
				searchBugIn(cell->objects, this, &min_dist, &target);
	}
	return target;
}

void Bug::OnEat(BugBase& first, BugBase& second)
{
	if (first.id > second.id)
	{
		second.disabled = true;
		second.visible = false;
		g_Game->objToCell[&second]->objects.remove(&second);
		g_Game->objToCell[&second] = nullptr;
	}
	else
	{
		first.disabled = true;
		first.visible = false;
		g_Game->objToCell[&first]->objects.remove(&first);
		g_Game->objToCell[&first] = nullptr;
	}
}
