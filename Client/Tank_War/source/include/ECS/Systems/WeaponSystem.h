#pragma once

#include "ECS/Core/System.h"

class WeaponSystem :public System
{
public:
	virtual void onAttach(World& world) override;
	virtual void update(float deltaTime) override;
};