#pragma once

#include "ECS/Core/Entity.h"
#include "ECS/Core/World.h"
#include "ECS/Components/TankProperty.h"
#include "box2d/box2d.h"

class TankFactory {
public:
	static Entity CreateLocalGameTank(World& world,
		TankProperty::TankOwner owner,
		float x, float y,
		int width, int height,
		float rotation
	);

	static Entity CreateClientTank(World& world,
		TankProperty::TankOwner owner, SyncID syncid, PlayerID playerid,
		float x, float y, int width, int height,
		float rotation);

private:
	static int16 getPhysicsGroupIndex();
};