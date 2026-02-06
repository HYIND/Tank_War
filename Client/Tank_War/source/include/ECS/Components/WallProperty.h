#pragma once

#include "ECS/Core/IComponent.h"

struct WallProperty :public IComponent
{
	enum WallType { BRICK, IRON };

	WallType type = WallType::BRICK;
	int width = 0;
	int height = 0;

	WallProperty() {}
	WallProperty(WallType type, int width, int height)
		: type(type), width(width), height(height) {
	}
};