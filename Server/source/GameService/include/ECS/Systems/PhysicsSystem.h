#pragma once

#include "ECS/Core/Entity.h"
#include "ECS/Core/System.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/BoundaryPhysisc.h"
#include "box2d/box2d.h"
#include "Helper/math2d.h"

class PhysicsSystem :public System
{

public:
	PhysicsSystem();
	virtual void fixedUpdate(float dt) override;

	std::vector<Entity> QueryAABB(const Pos2& pos1, const Pos2& pos2);	//左上，右下
	std::vector<Entity> RayCast(const Line2& line);
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