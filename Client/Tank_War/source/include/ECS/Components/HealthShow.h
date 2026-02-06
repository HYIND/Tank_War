#pragma once

#include "ECS/Components/RenderBase.h"
#include "ECS/Core/IComponent.h"

struct HealthShow : public RenderBase
{
	int width = 0;
	int height = 0;

	HealthShow() {}
	HealthShow(int width, int height) :width(width), height(height) {}
};