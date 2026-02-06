#pragma once

#include "ECS/Components/Transform.h"
#include "Helper/math2d.h"
#include "ECS/Core/IComponent.h"

enum class RenderLayer :int
{
	MapLayer = -10000,
	GameWallLayer = -2000,
	LayerDefault = 0,
	PropLayer = 1000,
	TankLayer = 2000,
	HealthShow = 5000,
	UILayer = 10000
};

// 可渲染组件基类
struct RenderBase :public IComponent
{
	int layer = (int)RenderLayer::LayerDefault;		// 渲染层级
	int internalZOrder = 0;							// 层级内zorder

	Vec2 offset{ 0,0 };	//渲染中心相对于Transform的偏移
};
