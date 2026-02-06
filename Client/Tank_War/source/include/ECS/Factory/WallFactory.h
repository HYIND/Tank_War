#pragma once

#include "ECS/Core/Entity.h"
#include "ECS/Core/World.h"
#include "ECS/Components/WallProperty.h"
#include "GameDataDef.h"

class WallFactory {
public:
	static Entity CreateWall(World& world, WallProperty::WallType type, float x, float y, int width, int height);

	static Entity CreateClientWall(World& world,
		SyncID syncid, WallProperty::WallType type,
		float x, float y, float rotation,
		int width, int height,
		int currenthealth, int maxhealth
	);
};