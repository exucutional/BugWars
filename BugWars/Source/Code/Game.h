#pragma once
#include "GameBase/GameBase.h"
#include "Globals.h"

constexpr int MapCellSize = 1024;
typedef long cell_dim_t;

struct Tank;

struct MapCell
{
	MapCell() : x(0), y(0), size(MapCellSize), neighbours {nullptr}, objects {} {}
	cell_dim_t x, y, size;						//  0
	MapCell* neighbours[4];						//1 x 2
	std::list<GameObject*> objects;				//  3 
};

struct Game : public GameBase
{
	Game();
	virtual ~Game();

	virtual void OnUpdate(float dt) override;
	virtual void OnRender() const override;
	virtual void AddObject(GameObject* object) override;

	virtual void OnBugsSpawned() override;
	MapCell* GetMapCell(cell_dim_t x, cell_dim_t y);
	std::unordered_set<MapCell*> GetMapCellNeighbours(MapCell* cell, int level);
	MapCell* TraverseMapX(MapCell* start, cell_dim_t x);
	MapCell* TraverseMapY(MapCell* start, cell_dim_t y);
	void UpdateMap(MapCell* cell);

	std::vector<GameObject*> objects;
	MapCell* map_cell;
	std::unordered_set<MapCell*> updated_cells;
	std::map<std::pair<cell_dim_t, cell_dim_t>, MapCell*> map;
};