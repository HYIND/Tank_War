#pragma once

#include "ECS/Core/System.h"

class ClientInputSystem :public System
{
public:
	virtual void update(float deltaTime) override;
	virtual void preUpdate(float deltaTime) override;
};