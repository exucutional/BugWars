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

BugBase* Tank::GetBugToShoot() const
{
	Bug* target = nullptr;
	float min_dist = std::numeric_limits<float>::max();
	for (auto obj : g_Game->objects)
	{
		if (auto bug = dynamic_cast<Bug*>(obj))
		{
			if (bug->disabled)
				continue;

			float dist = position.Distance(bug->position);
			if (dist < min_dist)
			{
				min_dist = dist;
				target = bug;
			}
		}
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
