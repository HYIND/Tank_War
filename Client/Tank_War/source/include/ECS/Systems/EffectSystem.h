#pragma once

#include "ECS/Core/System.h"
#include "Helper/math2d.h"

class EffectSystem :public System
{
public:
	virtual void onAttach(World& world) override;

private:
	void spawnExplosion(Pos2 position, int width, int height);
};