#pragma once
#include "GameBase/BulletBase.h"

struct Bullet : public BulletBase
{
	DEFINE_RTTI;
	Bullet();
	virtual void OnStart(Point direction) override;
	virtual void OnUpdate(float dt) override;
	virtual void OnLifeEnd() override;
};