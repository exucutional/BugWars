#pragma once
#include "GameBase/TankBase.h"

struct Tank : public TankBase
{
	DEFINE_RTTI;
	Tank();
	virtual void OnUpdate(float dt) override;
	// Get a bug to shoot 
	virtual BugBase* GetBugToShoot() const override;
	// Calculate shooting direction from the tank's center
	virtual Point CalcShootDirection(Point target_pos, Point target_dir, float target_vel, float bullet_vel) const override;

};