#pragma once
#include "Point.h"
#include "Globals.h"

struct GameObject;
struct TankBase;
struct BugBase;
struct BulletBase;
struct Texture;
struct Framework;

struct TankRoutePoint
{
	Point destination;
	int numFramesToGo = 0;
};
using TankRoute = std::vector<TankRoutePoint>;

struct GameBase
{
	struct GameObjectCreators
	{
		std::function<TankBase*()> tank_creator;
		std::function<BugBase*()> bug_creator;
		std::function<BulletBase*()> bullet_creator;
	};

	GameBase(GameObjectCreators&&);
	virtual ~GameBase();

	void Init(Framework& framework);
	void Update(float dt);
	void Render() const;

	virtual void OnUpdate(float dt) = 0;
	virtual void OnRender() const = 0;
	virtual void AddObject(GameObject* object) = 0;

	void SpawnBugs();
	virtual void OnBugsSpawned() = 0;
	void DoRender(GameObject* object) const;

	void OnBugUpdate_Begin(BugBase* bug);
	void OnBugUpdate_End(BugBase* bug);

	static const uint s_SpawnPeriodFrames = 100;

	Framework* framework = nullptr;
	std::default_random_engine rng{ 0 };
	TankRoute tankRoute;
	TankBase* tank = nullptr;
	GameObjectCreators creators;
	Texture* tankTexture = nullptr;
	Texture* bugTexture = nullptr;
	Texture* bulletTexture = nullptr;

	std::function<void(BugBase*)> onBugUpdate_Begin;
	std::function<void(BugBase*)> onBugUpdate_End;
};