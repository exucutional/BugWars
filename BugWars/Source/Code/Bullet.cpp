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
	g_Game->UpdateObj(this);
	cell_dim_t x_cell = (cell_dim_t)position.x / MapCellSize;
	cell_dim_t y_cell = (cell_dim_t)position.y / MapCellSize;
	auto cell = g_Game->GetMapCell(x_cell, y_cell, false);
	auto compare = [this](MapCell* c1, MapCell* c2) { return g_Game->GetDistToCell(position, c1) < g_Game->GetDistToCell(position, c2); };
	std::multiset<MapCell*, decltype(compare)> cells(compare);
	g_Game->GetMapCellNeighbours(cells, cell, 1, position);
	cells.insert(cell);
	for (auto& neighbour : cells)
		for (auto& object : neighbour->objects)
			if (!object->disabled)
				if (object->GetRTTI() == Bug::s_RTTI)
					if (object->position.Distance(position) < object->GetRadius())
					{
						g_Game->tank->score++;
						object->disabled = true;
						object->visible = false;
						g_Game->objToCell[object]->objects.remove(object);
						g_Game->objToCell[object] = nullptr;
						disabled = true;
						visible = false;
						g_Game->objToCell[this]->objects.remove(this);
						g_Game->objToCell[this] = nullptr;
						return;
					}
}

void Bullet::OnLifeEnd()
{
	disabled = true;
	visible = false;
	g_Game->objToCell[this]->objects.remove(this);
	g_Game->objToCell[this] = nullptr;
}
