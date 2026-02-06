#pragma once

#include "ECS/Core/System.h"
#include "ECS/Components/AllComponent.h"
#include "box2d/box2d.h"

class PredictionSystem : public System
{
public:
	PredictionSystem();
	virtual void fixedUpdate(float dt) override;

private:
	void checkNewEntity();
	void collisionCheck(float dt);
	void processCollisions();

	void createBody(Entity entity, Physics& physics, Transform& trans);
	void createBoundary(Entity entity, BoundaryPhysisc& boundaryphysics);

private:
	b2World phyWorld;
	const float PPM = 10.0f;
};