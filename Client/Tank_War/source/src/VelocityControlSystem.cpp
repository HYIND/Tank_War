#include "ECS/Systems/VelocityControlSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/Controller.h"
#include "ECS/Components/Movement.h"

void VelocityControlSystem::update(float deltaTime)
{
}

void VelocityControlSystem::preUpdate(float deltaTime)
{
	// 处理有Controller的实体，设置Movement的速度
	auto entities = m_world->getEntitiesWith<Controller, Movement>();

	for (auto entity : entities)
	{

		auto& contoller = entity.getComponent<Controller>();
		auto& movement = entity.getComponent<Movement>();

		switch (contoller.moveDirection)
		{
		case Controller::MoveDirection::NONE:
		{
			movement.setCurrentMoveSpeed(0);
			break;
		}
		case Controller::MoveDirection::FORWARD:
		{
			movement.setCurrentMoveSpeed(movement.maxMoveSpeed);
			break;
		}
		case Controller::MoveDirection::BACKWARD:
		{
			movement.setCurrentMoveSpeed(-movement.maxMoveSpeed);
			break;
		}
		default:
			break;
		}

		switch (contoller.rotationDirection)
		{
		case Controller::RotationDirection::NONE:
		{
			movement.setRotationSpeed(0);
			break;
		}
		case Controller::RotationDirection::LEFT:
		{
			movement.setRotationSpeed(-movement.maxRotationSpeed);
			break;
		}
		case Controller::RotationDirection::RIGHT:
		{
			movement.setRotationSpeed(movement.maxRotationSpeed);
			break;
		}
		default:
			break;
		}
	}
}
