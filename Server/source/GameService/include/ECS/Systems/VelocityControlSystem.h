#pragma once

#include "ECS/Core/System.h"

class VelocityControlSystem :public System
{
public:
	virtual void update(float dt)override;
	virtual void preUpdate(float deltaTime) override;
};