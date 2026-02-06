#pragma once

#include "Manager/ResourceManager.h"
#include "ECS/Core/IComponent.h"

struct PropProperty :public IComponent
{
	enum PropType {
		HEALTH_PACK,    // 医疗包
		FAST_BULLET,    // 快速子弹
		SHIELD,         // 护盾
		MULTI_SHOT      // 多重射击
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