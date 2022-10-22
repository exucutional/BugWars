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
}

template<typename Container>
static void searchBugIn(Container c, const Bug* me, float* min_dist, Bug** target)
{
	for (auto object : c)
	{
		if (auto bug = dynamic_cast<Bug*>(object))
		{
			if (bug == me)
				continue;

			if (bug->disabled)
				continue;

			if (bug->id > me->id)
				continue; // Can't eat that

			float dist = me->position.Distance(bug->position);
			if (dist == 0)	// Do not allow zero vectors
				continue;

			if (dist < *min_dist)
			{
				*min_dist = dist;
				*target = bug;
			}
		}
	}
}

BugBase* Bug::FindBugToEat() const
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), __FUNCTION__);
	Bug* target = nullptr;
	float min_dist = std::numeric_limits<float>::max();
	cell_dim_t x_cell = (cell_dim_t)position.x / g_Game->map_cell->size;
	cell_dim_t y_cell = (cell_dim_t)position.y / g_Game->map_cell->size;
	auto cell = g_Game->GetMapCell(x_cell, y_cell);
	int level = 1;
	int maxLevel = 3;
	searchBugIn(cell->objects, this, &min_dist, &target);
	while (!target && level <= maxLevel)
	{
		auto neighbours = g_Game->GetMapCellNeighbours(cell, level++);
		for (auto cell : neighbours)
			if (cell)
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
	}
	else
	{
		first.disabled = true;
		first.visible = false;
	}
}
