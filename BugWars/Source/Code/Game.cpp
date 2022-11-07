#include "pch.h"
#include "Game.h"
#include "GameBase/Log.h"
#include "Tank.h"
#include "Bug.h"
#include "Bullet.h"
#include <array>

Game* g_Game;

Game::Game()
	: GameBase({ [] {return new Tank; },
				 [] {return new Bug; },
				 [] {return new Bullet; } }),
	map_cell(new MapCell()),
	map_dim(0)
{
	map.insert({ std::make_pair(map_cell->x, map_cell->y), map_cell });
	g_Game = this;
}

void Game::OnUpdate(float dt)
{
	PIXScopedEvent(PIX_COLOR_INDEX(5), __FUNCTION__);
	for (int i = 0; i < objects.size(); i++)
		if (!objects[i]->disabled)
			objects[i]->Update(dt);
}

void Game::OnRender() const
{
	PIXScopedEvent(PIX_COLOR_INDEX(4), __FUNCTION__);
	for (auto obj : objects)
		if (obj->visible)
			DoRender(obj);
}

void Game::AddObject(GameObject* object)
{
	objects.push_back(object);
	cell_dim_t x_cell = (cell_dim_t)object->position.x / MapCellSize;
	cell_dim_t y_cell = (cell_dim_t)object->position.y / MapCellSize;
	auto cell = GetMapCell(x_cell, y_cell, true);
	cell->objects.push_back(object);
	objToCell[object] = cell;
	//if (object->GetRTTI() == Bug::s_RTTI)
		//Log("I'm a bug");
}

void Game::OnBugsSpawned()
{
}

float Game::GetDistToCell(const Point& p, MapCell* cell)
{
	float x = (p.x < cell->x * MapCellSize) ? cell->x * MapCellSize - p.x : p.x - (cell->x + 1) * MapCellSize;
	float y = (p.y < cell->y * MapCellSize) ? cell->y * MapCellSize - p.y : p.y - (cell->y + 1) * MapCellSize;
	if (x < 0.0f && y < 0.0f)
		return 0.0f;

	if (x < 0.0f)
		return y;

	if (y < 0.0f)
		return x;

	return std::min(x, y);
}

MapCell* Game::GetMapCell(cell_dim_t x, cell_dim_t y, bool create)
{
	if (create)
		map_dim = std::max(map_dim, std::max(x, y));

	auto cell = map.find(std::make_pair(x, y));
	if (cell != std::end(map))
		return cell->second;

	if (!create)
		return nullptr;

	auto new_cell = new MapCell();
	new_cell->x = x;
	new_cell->y = y;
	map.insert({ std::make_pair(x, y), new_cell });
	return new_cell;
}

void Game::UpdateObj(GameObject* object)
{
	cell_dim_t cell_x = (cell_dim_t)object->position.x / MapCellSize;
	cell_dim_t cell_y = (cell_dim_t)object->position.y / MapCellSize;
	auto cell = objToCell[object];
	if (cell_x != cell->x || cell_y != cell->y)
	{
		auto new_cell = GetMapCell(cell_x, cell_y, true);
		new_cell->objects.push_back(object);
		objToCell[object] = new_cell;
		if (cell)
			cell->objects.remove(object);
	}
}

Game::~Game()
{
	for (auto obj : objects)	// Fix memory leak
		delete(obj);

	for (auto& key_cell : map)
		delete(key_cell.second);
}
