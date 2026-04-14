#pragma once

#include "ECS/Core/IComponent.h"

struct PropProperty :public IComponent
{
	enum PropType {
		HEALTH_PACK = 0,    // 医疗包
		ENERGY_WAVE
	};

	PropType type;
	int width;
	int height;
	float duration;     // 效果持续时间（秒）

	PropProperty() {}
	PropProperty(PropType t, int width, int height, float dur = 10.0f)
		: type(t), width(width), height(height), duration(dur) {
	}
};