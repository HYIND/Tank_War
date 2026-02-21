#pragma once

#include <iostream>
#include "Helper/math2d.h"
#include "ECS/Core/IComponent.h"
#include "ECS/Components/Transform.h"

struct AIControl :public IComponent
{
	Pos2 lastPos = Pos2(0, 0);
	float lastRot = 0.f;

	// 平滑方向变化
	Vec2 smoothedDirection = Vec2(1.f, 0.f);
	float SMOOTH_FACTOR = 0.2f;  // 平滑因子，值越小越平滑，但反应越慢
	float MIN_CHANGE = 5.f;    // 最小变化阈值，低于此值不改变方向

   // 计时器
	float timeAccumulator = 0.f;
	float decisionIntervalms = 50.f;  // 做决策的间隔

	AIControl() {};
	void OnAdd(Entity& e)override
	{
		if (auto* trans = e.tryGetComponent<Transform>())
		{
			lastPos = trans->position;
			lastRot = trans->rotation;
			float rad = trans->rotation * M_PI / 180.0f;
			smoothedDirection = Vec2(std::cos(rad), std::sin(rad));
		}
	}
};