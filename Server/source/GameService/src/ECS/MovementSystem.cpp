#include "ECS/Systems/MovementSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Movement.h"

#define _USE_MATH_DEFINES
#include <math.h>

void handleMove(Transform& trans, Movement& movement, float deltaTime)
{
	//float rate = deltaTime / 1000.f;

	//if (movement.currentRotationSpeed != 0.f)
	//{
	//	float ori_rotate = trans.rotation;
	//	trans.rotation = ori_rotate - movement.currentRotationSpeed * rate;
	//}

	//if (movement.currentMoveSpeed != 0.f)
	//{
	//	float ori_locationX = trans.position.x;
	//	float ori_locationY = trans.position.y;
	//	trans.position.x = ori_locationX + sin(trans.rotation * M_PI / 180) * movement.currentMoveSpeed * rate;
	//	trans.position.y = ori_locationY - cos(trans.rotation * M_PI / 180) * movement.currentMoveSpeed * rate;
	//}
}

void MovementSystem::update(float deltaTime)
{
	auto& world = getWorld();
	std::vector<Entity> entities = world.getEntitiesWith<Transform, Movement>();
	for (auto& entity : entities)
	{
		auto& transform = world.getComponent<Transform>(entity);
		auto& movement = world.getComponent<Movement>(entity);
		handleMove(transform, movement, deltaTime);
	}
}
