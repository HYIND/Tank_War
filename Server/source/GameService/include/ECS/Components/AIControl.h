#pragma once

#include <iostream>
#include "Helper/math2d.h"
#include "ECS/Core/IComponent.h"

struct AIControl :public IComponent
{
	enum class State {
		IDLE,
		PATROLLING,
		CHASING,
		ATTACKING,
		FLEEING
	};

	State currentState = State::IDLE;
	float detectionRange = 10.0f;
	float attackRange = 2.0f;
	float moveSpeed = 3.0f;

	// AI目标
	struct Target {
		uint32_t entityId = 0;  // 目标实体ID
		Vec2 position;          // 目标位置
		bool valid = false;
	} target;

	// 计时器
	float stateTimer = 0.0f;
	float decisionInterval = 1.0f;  // 做决策的间隔
};