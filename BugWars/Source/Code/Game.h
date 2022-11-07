#pragma once
#include "GameBase/GameBase.h"
#include "Globals.h"

constexpr int MapCellSize = 2048-256;
typedef long cell_dim_t;

struct Tank;

struct MapCell
{
	MapCell() : x(0), y(0), objects {} {}
	cell_dim_t x, y;
	std::list<GameObject*> objects;
};

struct Game : public GameBase
{
	Game();
	virtual ~Game();

	virtual void OnUpdate(float dt) override;
	virtual void OnRender() const override;
	virtual void AddObject(GameObject* object) override;

	virtual void OnBugsSpawned() override;
	MapCell* GetMapCell(cell_dim_t x, cell_dim_t y, bool create);
	float GetDistToCell(const Point& p, MapCell* cell);
	template <typename T>
	void GetMapCellNeighbours(T& cells, MapCell* cell, int level, const Point& pos);
	void UpdateObj(GameObject* object);

	cell_dim_t map_dim;
	std::vector<GameObject*> objects;
	MapCell* map_cell;
	std::unordered_set<MapCell*> updated_cells;
	std::map<std::pair<cell_dim_t, cell_dim_t>, MapCell*> map;
	std::map<GameObject*, MapCell*> objToCell;
};

template <typename T>
void Game::GetMapCellNeighbours(T& cells, MapCell* cell, int level, const Point& pos)
{
	for (int i = 0; i < 3 + 2 * (level - 1); i++)
	{
		if (cell->x - level + i >= 0 && cell->y - level >= 0)
		{
			auto neighbour = GetMapCell(cell->x - level + i, cell->y - level, false);
			if (neighbour && !neighbour->objects.empty())
				cells.insert(neighbour);
		}
		if (cell->x - level >= 0 && cell->y - level + i >= 0)
		{
			auto neighbour = GetMapCell(cell->x - level, cell->y - level + i, false);
			if (neighbour && !neighbour->objects.empty())
				cells.insert(neighbour);
		}
		if (cell->x + level >= 0 && cell->y + level - i >= 0)
		{
			auto neighbour = GetMapCell(cell->x + level, cell->y + level - i, false);
			if (neighbour && !neighbour->objects.empty())
				cells.insert(neighbour);
		}
		if (cell->x + level - i >= 0 && cell->y + level >= 0)
		{
			auto neighbour = GetMapCell(cell->x + level - i, cell->y + level, false);
			if (neighbour && !neighbour->objects.empty())
				cells.insert(neighbour);
		}
	}
}
