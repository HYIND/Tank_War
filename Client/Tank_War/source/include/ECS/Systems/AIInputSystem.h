#pragma once

#include "ECS/Components/AllComponent.h"
#include "ECS/Core/System.h"

class AIInputSystem :public System
{
public:
	virtual void preUpdate(float deltaTime) override;

private:
	void handleAIInputToTank(Entity AITank);
	void drawDebugInfo(Entity AITank);
};