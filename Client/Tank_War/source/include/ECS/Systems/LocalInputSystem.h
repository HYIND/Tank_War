#pragma once

#include "ECS/Core/System.h"

class LocalInputSystem :public System
{
public:
	virtual void update(float deltaTime) override;
	virtual void preUpdate(float fixedDeltaTime) override;
};