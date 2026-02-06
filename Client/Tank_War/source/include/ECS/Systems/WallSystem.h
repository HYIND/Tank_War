#pragma once

#include "ECS/Core/System.h"

class WallSystem :public System
{
public:
	virtual void onAttach(World& world) override;
};