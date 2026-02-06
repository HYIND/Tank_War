#pragma once

#include <stdint.h>
#include "ECS/Core/IComponent.h"

struct PlayerInput :public IComponent
{
	enum InputState {
		FORWARD = 1 << 0,
		BACKWARD = 1 << 1,
		LEFT = 1 << 2,
		RIGHT = 1 << 3,
		FIRE = 1 << 4
	};

	uint8_t inputState;      // 用位掩码存储输入状态

	PlayerInput() : inputState(0) {}

	void setInput(InputState state, bool pressed) {
		if (pressed) {
			inputState |= state;
		}
		else {
			inputState &= ~state;
		}
	}

	bool isPressed(InputState state) const {
		return (inputState & state) != 0;
	}
};