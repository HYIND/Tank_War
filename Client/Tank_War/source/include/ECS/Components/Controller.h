#pragma once

#include "ECS/Core/IComponent.h"

// 控制器组件
struct Controller :public IComponent
{
	enum class MoveDirection :int { NONE = 0, FORWARD, BACKWARD };
	enum class RotationDirection :int { NONE = 0, LEFT, RIGHT };

	MoveDirection moveDirection = MoveDirection::NONE;				//移动意图
	RotationDirection rotationDirection = RotationDirection::NONE;	//旋转意图

	// 攻击意图
	bool wantToFire = false;	// 是否想射击

	void setWantToMove(MoveDirection direction) { moveDirection = direction; }
	void setWantToRotate(RotationDirection direction) { rotationDirection = direction; };
	void setWantToFire(bool enabled) { wantToFire = enabled; }
};
