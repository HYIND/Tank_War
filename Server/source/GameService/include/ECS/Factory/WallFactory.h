#pragma once

#include "ECS/Core/Entity.h"
#include "ECS/Core/World.h"
#include "ECS/Components/WallProperty.h"

class WallFactory
{
public:
	static Entity CreateServerWall(World &world,
								   WallProperty::WallType type,
								   float x, float y, float rotation,
								   int width, int height,
								   int maxhealth);
};