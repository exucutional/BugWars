#include "pch.h"
#include "Tank.h"
#include "Globals.h"
#include "Game.h"
#include "Bullet.h"
#include "Bug.h"

IMPLEMENT_RTTI(Tank);

Tank::Tank()
{
	visible = true;
	disabled = false;
}

void Tank::OnUpdate(float dt)
{
	g_Game->UpdateObj(this);
}

template<typename Container>
static void searchBugIn(Container& c, const Tank* me, float* min_dist, Bug** target)
{
	for (auto& object : c)
	{
		if (object->GetRTTI() == Bug::s_RTTI)
		{
			if (object->disabled)
				continue;

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

BugBase* Tank::GetBugToShoot() const
{
	Bug* target = nullptr;
	float min_dist = std::numeric_limits<float>::max();
	cell_dim_t x_cell = (cell_dim_t)position.x / MapCellSize;
	cell_dim_t y_cell = (cell_dim_t)position.y / MapCellSize;
	auto cell = g_Game->GetMapCell(x_cell, y_cell, false);
	int level = 1;
	int maxLevel = std::max(std::max(cell->x, g_Game->map_dim - cell->x), std::max(cell->y, g_Game->map_dim - cell->y));
	maxLevel = std::max(1, maxLevel);
	auto compare = [this](MapCell* c1, MapCell* c2) { return g_Game->GetDistToCell(position, c1) < g_Game->GetDistToCell(position, c2); };
	std::multiset<MapCell*, decltype(compare)> cells(compare);
	while (min_dist > (level - 1) * MapCellSize && level <= maxLevel)
	{
		if (level == 1)
			searchBugIn(cell->objects, this, &min_dist, &target);

		g_Game->GetMapCellNeighbours(cells, cell, level++, position);
		for (auto cell : cells)
			if (cell && g_Game->GetDistToCell(position, cell) < min_dist)
				searchBugIn(cell->objects, this, &min_dist, &target);

		cells.clear();
	}
	return target;
}

Point Tank::CalcShootDirection(Point target_pos, Point target_dir, float target_vel, float bullet_vel) const
{
	Point dir = target_pos - position;
	if (dir.x * target_dir.y == dir.y * target_dir.x)
		return dir;

	float bullet_vel2 = bullet_vel * bullet_vel;
	float target_vel2 = target_vel * target_vel;
	float cosv = dir.Dot(target_dir) / dir.Length() / target_dir.Length();
	float cosv2 = cosv * cosv;
	float sqrtval = target_vel2 * cosv2 + bullet_vel2 - target_vel2;

	if (sqrtval < 0)
		return dir;

	float dist = position.Distance(target_pos);
	float dist2 = dist * dist;
	float T = -target_vel * dist * cosv + dist * sqrt(sqrtval);

	if (bullet_vel2 != target_vel2)
		T /= bullet_vel2 - target_vel2;
	else
		T = 1.0f;

	return dir + target_dir.Normalized() * target_vel * T;
}
