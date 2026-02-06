#pragma once

#include "ECS/Components/Transform.h"
#include <algorithm>
#include "ECS/Core/IComponent.h"

struct Movement :public IComponent
{
	float maxMoveSpeed;             // 移动速度（像素/秒）
	float maxRotationSpeed;         // 最大旋转速度（度/秒）

	float currentMoveSpeed = 0.f;
	float currentRotationSpeed = 0.f;

	Movement(float maxmoveSpd = 400.f, float maxrotSpd = 180.f)
		: maxMoveSpeed(std::abs(maxmoveSpd)), maxRotationSpeed(std::abs(maxrotSpd)) {
	}
	void setCurrentMoveSpeed(float movespeed)
	{
		currentMoveSpeed = std::clamp(movespeed, -maxMoveSpeed, maxMoveSpeed);
	}
	void setRotationSpeed(float rotspeed)
	{
		currentRotationSpeed = std::clamp(rotspeed, -maxRotationSpeed, maxRotationSpeed);
	}
};