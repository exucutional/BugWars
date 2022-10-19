#include "pch.h"
#include "Game.h"
#include "GameBase/Log.h"
#include "Tank.h"
#include "Bug.h"
#include "Bullet.h"

Game* g_Game;

Game::Game()
	: GameBase({ [] {return new Tank; },
				 [] {return new Bug; },
				 [] {return new Bullet; } })
{
	g_Game = this;
}

void Game::OnUpdate(float dt)
{
	PIXScopedEvent(PIX_COLOR_INDEX(5), __FUNCTION__);
	for (int i = 0; i < objects.size(); i++)		// Why does range based for give dead pointer?
		if (!objects[i]->disabled)					// And classic for loop do not?
			objects[i]->Update(dt);
}

void Game::OnRender() const
{
	for (auto obj : objects)
		if (obj->visible)
			DoRender(obj);
}

void Game::AddObject(GameObject* object)
{
	objects.push_back(object);
	if (object->GetRTTI() == Bug::s_RTTI)
		Log("I'm a bug");
}

void Game::OnBugsSpawned()
{
}

Game::~Game()
{
	for (auto obj : objects)	// Fix memory leak
		delete(obj);
}
