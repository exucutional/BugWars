#include "pch.h"
#include "Bullet.h"
#include "Game.h"
#include "Bug.h"
#include "Tank.h"

IMPLEMENT_RTTI(Bullet);

Bullet::Bullet()
{
	visible = true;
	disabled = false;
}

void Bullet::OnStart(Point)
{
}

void Bullet::OnUpdate(float dt)
{
	for (auto object : g_Game->objects)
		if (!object->disabled)
			if (auto bug = dynamic_cast<Bug*>(object))
				if (bug->position.Distance(position) < bug->GetRadius())
				{
					g_Game->tank->score++;
					bug->disabled = true;
					bug->visible = false;
					disabled = true;
					visible = false;
					return;
				}
}

void Bullet::OnLifeEnd()
{
	disabled = true;
	visible = false;
}
