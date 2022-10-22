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
}

template<typename Container>
static void searchBugIn(Container c, const Tank* me, float* min_dist, Bug** target)
{
	for (auto object : c)
	{
		if (auto bug = dynamic_cast<Bug*>(object))
		{
			if (bug->disabled)
				continue;

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

BugBase* Tank::GetBugToShoot() const
{
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
