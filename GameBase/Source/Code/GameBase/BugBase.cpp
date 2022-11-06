#include "pch.h"
#include "BugBase.h"
#include "Globals.h"
#include "GameBase.h"

const float BugBase::s_Velocity = 100.0f;

void BugBase::Update(float dt)
{
	g_GameBase->OnBugUpdate_Begin(this); // For testing purposes

	BugBase* target = FindBugToEat();
	if (!target)
	{
		g_GameBase->OnBugUpdate_End(this); // For testing purposes
		return;
	}

	auto vec = target->position - position;
	float len2 = vec.Length2();
	float r = GetRadius();
	float _2r2 = r * r * 2 * 2;
	if (len2 < _2r2)
		OnEat(*this, *target);

	direction = vec.Normalized();
	position = position + direction * s_Velocity * dt;
	if (direction.x != 0.0f)
		angle = 90.0f + atan2(direction.y, direction.x) * 180.0f / std::numbers::pi_v<float>;

	OnUpdate(dt);

	g_GameBase->OnBugUpdate_End(this); // For testing purposes
}

Texture* BugBase::GetTexture() const
{
	return g_GameBase->bugTexture;
}
