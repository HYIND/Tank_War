#pragma once

#include "ECS/Core/System.h"
#include "Helper/math2d.h"
#include "ECS/Core/Entity.h"

class PropSystem :public System
{
public:
	virtual void onAttach(World& world) override;

private:
	void processPropCollision(Entity prop, Entity picker, Pos2 point);
	void processHealthPack(Entity prop, Entity picker);
};