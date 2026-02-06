#pragma once

#include "ECS/Core/System.h"
#include "Helper/math2d.h"
#include "ECS/Core/Entity.h"

class BulletSystem :public System
{
public:
	virtual void onAttach(World& world) override;

private:
	void processBulletCollision(Entity bullet, Entity target, Pos2 point);
	void processBulletBounce(Entity bullet);
};