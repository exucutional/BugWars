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
	map_cell(new MapCell())
{
	map.insert({ std::make_pair(map_cell->x, map_cell->y), map_cell });
	g_Game = this;
}

void Game::OnUpdate(float dt)
{
	PIXScopedEvent(PIX_COLOR_INDEX(5), __FUNCTION__);
	for (int i = 0; i < objects.size(); i++)		// Why does range based for give dead pointer?
		if (!objects[i]->disabled)					// And classic for loop do not?
			objects[i]->Update(dt);

	UpdateMap(map_cell);
	updated_cells.clear();
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
	cell_dim_t x_cell = (cell_dim_t)object->position.x / map_cell->size;
	cell_dim_t y_cell = (cell_dim_t)object->position.y / map_cell->size;
	auto cell = GetMapCell(x_cell, y_cell);
	cell->objects.push_back(object);
	//if (object->GetRTTI() == Bug::s_RTTI)
		//Log("I'm a bug");
}

void Game::OnBugsSpawned()
{
}

static void searchNeighbours(std::unordered_set<MapCell*>& cells, MapCell* origin, MapCell* cell, int level)
{
	if (!cells.contains(cell) && (abs(cell->x - origin->x) == level || abs(cell->y - origin->y) == level))
		cells.insert(cell);

	for (auto neighbour : cell->neighbours)
		if (neighbour && abs(neighbour->x - origin->x) <= level && abs(neighbour->y - origin->y) <= level
		&& !cells.contains(neighbour)
		&& (abs(neighbour->x - origin->x) > abs(cell->x - origin->x) || abs(neighbour->y - origin->y) > abs(cell->y - origin->y)))
			searchNeighbours(cells, origin, neighbour, level);

}

std::unordered_set<MapCell*> Game::GetMapCellNeighbours(MapCell* cell, int level)
{
	std::unordered_set<MapCell*> cells {};
	searchNeighbours(cells, cell, cell, level);
	return cells;
}

float Game::GetDistToCell(Point p, MapCell* cell)
{
	float x = (p.x < cell->x * cell->size) ? cell->x * cell->size - p.x : p.x - (cell->x + 1) * cell->size;
	float y = (p.y < cell->y * cell->size) ? cell->y * cell->size - p.y : p.y - (cell->y + 1) * cell->size;
	if (x < 0.0f && y < 0.0f)
		return 0.0f;

	if (x < 0.0f)
		return y;

	if (y < 0.0f)
		return x;

	return std::min(x, y);
}

MapCell* Game::TraverseMapX(MapCell* start, cell_dim_t x)
{
	auto cell = start;
	while (x != cell->x)
	{
		int next_i = (x < cell->x) ? 1 : 2;
		cell_dim_t next_x = (x < cell->x) ? cell->x - 1 : cell->x + 1;
		if (!cell->neighbours[next_i])
		{
			auto new_cell_i = map.find(std::make_pair(next_x, cell->y));
			MapCell* new_cell = nullptr;
			if (new_cell_i != std::end(map))
				new_cell = new_cell_i->second;
			else
				new_cell = new MapCell();

			new_cell->x = next_x;
			new_cell->y = cell->y;
			new_cell->neighbours[3 - next_i] = cell;
			cell->neighbours[next_i] = new_cell;
			map.insert({ std::make_pair(new_cell->x, new_cell->y), new_cell });
		}
		cell = cell->neighbours[next_i];
	}
	return cell;
}

MapCell* Game::TraverseMapY(MapCell* start, cell_dim_t y)
{
	auto cell = start;
	while (y != cell->y)
	{
		int next_i = (y < cell->y) ? 0 : 3;
		cell_dim_t next_y = (y < cell->y) ? cell->y - 1 : cell->y + 1;
		if (!cell->neighbours[next_i])
		{
			auto new_cell_i = map.find(std::make_pair(cell->x, next_y));
			MapCell* new_cell = nullptr;
			if (new_cell_i != std::end(map))
				new_cell = new_cell_i->second;
			else
				new_cell = new MapCell();

			new_cell->y = next_y;
			new_cell->x = cell->x;
			new_cell->neighbours[3 - next_i] = cell;
			cell->neighbours[next_i] = new_cell;
			map.insert({ std::make_pair(new_cell->x, new_cell->y), new_cell });
		}
		cell = cell->neighbours[next_i];
	}
	return cell;
}

MapCell* Game::GetMapCell(cell_dim_t x, cell_dim_t y)
{
	auto cell = map.find(std::make_pair(x, y));
	if (cell != std::end(map))
		return cell->second;

	auto cellx = TraverseMapX(map_cell, x);
	auto celly = TraverseMapY(map_cell, y);
	auto cellxy = TraverseMapY(cellx, y);
	auto cellyx = TraverseMapX(celly, x);
	return cellxy;
}

void Game::UpdateMap(MapCell* cell)
{
	PIXScopedEvent(PIX_COLOR_INDEX(3), __FUNCTION__);
	if (updated_cells.contains(cell))
		return;
	if (cell->objects.size())
	{
		auto object_i = std::begin(cell->objects);
		while (object_i != std::end(cell->objects))
		{
			if ((*object_i)->disabled)
			{
				object_i = cell->objects.erase(object_i);
				continue;
			}
			cell_dim_t cell_x = (cell_dim_t)(*object_i)->position.x / cell->size;
			cell_dim_t cell_y = (cell_dim_t)(*object_i)->position.y / cell->size; 
			if (cell_x != cell->x || cell_y != cell->y )
			{
				GetMapCell(cell_x, cell_y)->objects.push_back(*object_i);
				object_i = cell->objects.erase(object_i);
			}
			else
			{
				++object_i;
			}
		}
	}
	updated_cells.insert(cell);
	if (cell->neighbours[2])
		UpdateMap(cell->neighbours[2]);
	if (cell->neighbours[3])
		UpdateMap(cell->neighbours[3]);
}

Game::~Game()
{
	for (auto obj : objects)	// Fix memory leak
		delete(obj);

	for (auto& key_cell : map)
		delete(key_cell.second);
}
