#pragma once

#include "ECS/Core/IComponent.h"
#include "box2d/box2d.h"

// 物理边界组件
struct BoundaryPhysisc :public IComponent
{
	int x1 = 0;
	int y1 = 0;
	int x2 = 0;
	int y2 = 0;

	b2Body* boundaryBody = nullptr;
	b2World* world = nullptr;

	BoundaryPhysisc() {};
	BoundaryPhysisc(int x1, int y1, int x2, int y2)
		:x1(x1), y1(y1), x2(x2), y2(y2)
	{
	}

	virtual void OnRemove() override
	{
		if (world && boundaryBody)
			world->DestroyBody(boundaryBody);
	}
};