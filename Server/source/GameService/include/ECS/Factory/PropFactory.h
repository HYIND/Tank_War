#pragma once

#include "ECS/Core/Entity.h"
#include "ECS/Core/World.h"
#include "ECS/Components/PropProperty.h"

class PropFactory
{
public:
	static Entity CreateServerProp(World &world,
								   PropProperty::PropType type, float duration,
								   float x, float y, float rotation,
								   int width, int height,
								   float lifetime = 0);
};