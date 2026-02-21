#pragma once

#include <iostream>
#include "Helper/math2d.h"
#include "ECS/Core/IComponent.h"

struct AIControl :public IComponent
{
	Pos2 lastPos = Pos2(0, 0);

	// 计时器
	float timeAccumulator = 0.f;
	float decisionIntervalms = 50.f;  // 做决策的间隔s
};