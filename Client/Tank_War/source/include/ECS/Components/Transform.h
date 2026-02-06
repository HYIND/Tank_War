#pragma once

#include "Helper/math2d.h"
#include "ECS/Core/IComponent.h"

struct Transform :public IComponent
{
	Pos2 position;
	float rotation;
	Vec2 scale;

	Transform(Pos2 pos = Pos2(0, 0), float rot = 0.f, Vec2 scl = Vec2(1, 1))
		: position(pos), rotation(rot), scale(scl) {
	}
};