#pragma once

#include "GameDataDef.h"
#include "ECS/Core/Entity.h"
#include "ECS/Core/World.h"
#include "ECS/Components/TankProperty.h"
#include "box2d/box2d.h"

class TankFactory
{
public:
	static Entity CreateServerTank(World &world, TankProperty::TankOwner owner, PlayerID playerid, float x, float y, int width, int height);

private:
	static int16 getPhysicsGroupIndex();
};