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
	cell_dim_t x_cell = (cell_dim_t)position.x / g_Game->map_cell->size;
	cell_dim_t y_cell = (cell_dim_t)position.y / g_Game->map_cell->size;
	auto cell = g_Game->GetMapCell(x_cell, y_cell);
	auto neighbours = g_Game->GetMapCellNeighbours(cell, 1);
	neighbours.insert(cell);
	for (auto neighbour : neighbours)
		for (auto object : neighbour->objects)
			if (!object->disabled)
				if (object->GetRTTI() == Bug::s_RTTI)
					if (object->position.Distance(position) < object->GetRadius())
					{
						g_Game->tank->score++;
						object->disabled = true;
						object->visible = false;
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
