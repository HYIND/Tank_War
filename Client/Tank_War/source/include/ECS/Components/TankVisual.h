#pragma once

#include "RenderBase.h"
#include "ECS/Core/IComponent.h"

struct TankVisual :public RenderBase
{
	enum class VisualState :int { NONE, BASIC, BIGGER, SMALLER };

	VisualState visualstate = VisualState::NONE;

	int width = 0;
	int height = 0;

	TankVisual() = default;

	TankVisual(VisualState visualstate, int width, int height) :
		visualstate(visualstate), width(width), height(height) {
	};
};