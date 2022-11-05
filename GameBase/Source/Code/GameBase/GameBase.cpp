#include "pch.h"
#include "GameBase.h"
#include "Globals.h"
#include "Texture.h"
#include "Framework.h"
#include "TankBase.h"
#include "BugBase.h"

GameBase* g_GameBase = nullptr;

const uint GameBase::s_SpawnPeriodFrames;

GameBase::GameBase(GameBase::GameObjectCreators&& creators_)
	: creators(creators_)
{
	g_GameBase = this;	
}

GameBase::~GameBase()
{
	delete tankTexture;
	delete bugTexture;
	delete bulletTexture;
}

void GameBase::Init(Framework& framework_)
{
	framework = &framework_;

	tankTexture = new Texture("Source/Data/Tank.png", framework->renderer);
	bugTexture = new Texture("Source/Data/Bug.png", framework->renderer);
	bulletTexture = new Texture("Source/Data/Bullet.png", framework->renderer);

	tankRoute.push_back(TankRoutePoint{ {6000, 6000}, 1500 });
	tankRoute.push_back(TankRoutePoint{ {3000, 0}, 500 });
	tankRoute.push_back(TankRoutePoint{ {6000, 3000}, 500 });
	tankRoute.push_back(TankRoutePoint{ {3000, 6000}, 1000 });
	tankRoute.push_back(TankRoutePoint{ {1500, 0}, 1000 });
	tankRoute.push_back(TankRoutePoint{ {0, 6000}, 500 });
	std::reverse(tankRoute.begin(), tankRoute.end());
	tank = creators.tank_creator();
	AddObject(tank);

	SpawnBugs();
}

void GameBase::SpawnBugs()
{
	const uint spawn_range = 100;
	std::uniform_int_distribution<int> range(0, spawn_range - 1);
	bool map[spawn_range][spawn_range];
	for (int i = 0; i < spawn_range; i++)
		memset(map[i], 0, spawn_range * sizeof(bool));

	const uint num_bugs = 1198;
	for (int i = 0; i < num_bugs; i++)
	{
		int x = 0;
		int y = 0;

		while (map[x][y])
		{
			x = range(rng);
			y = range(rng);
		}

		auto bug = creators.bug_creator();
		float r = bug->GetRadius() * 3;
		bug->position = Point(x * r, y * r);
		AddObject(bug);
		map[x][y] = true;
	}

	OnBugsSpawned();
}

void GameBase::DoRender(GameObject* obj) const
{
	auto tex = obj->GetTexture();
	auto pos = obj->position;

	pos = pos + framework->screenSize / 2;
	pos = pos - tank->position;

	auto size = tex->size;

	SDL_Rect r((int)round(pos.x - tex->size.x / 2),
		(int)round(pos.y - tex->size.y / 2),
		(int)tex->size.x,
		(int)tex->size.y);
	SDL_RenderCopyEx(framework->renderer, tex->texture, nullptr, &r, obj->angle, nullptr, SDL_FLIP_NONE);
}

void GameBase::Update(float dt)
{
	OnUpdate(dt);

	static int frames_since_last_spawn = 0;
	if (frames_since_last_spawn++ > s_SpawnPeriodFrames)
	{
		frames_since_last_spawn = 0;
		SpawnBugs();
	}
}

void GameBase::Render() const
{
	OnRender();
}

void GameBase::OnBugUpdate_Begin(BugBase* bug)
{
	if (onBugUpdate_Begin)
		onBugUpdate_Begin(bug);
}

void GameBase::OnBugUpdate_End(BugBase* bug)
{
	if (onBugUpdate_End)
		onBugUpdate_End(bug);
}
