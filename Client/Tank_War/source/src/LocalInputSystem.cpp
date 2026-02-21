#include "stdafx.h"
#include "ECS/Systems/LocalInputSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/PlayerInput.h"
#include "ECS/Components/TankProperty.h"
#include "ECS/Components/Controller.h"
#include "Helper/keymap.h"

bool Get_keymap(TankProperty::TankOwner owner, int key[5])
{
	if (owner == TankProperty::TankOwner::PLAYER1)
	{
		key[0] = key_map_p1[keybroad::UP];
		key[1] = key_map_p1[keybroad::DOWN];
		key[2] = key_map_p1[keybroad::LEFT];
		key[3] = key_map_p1[keybroad::RIGHT];
		key[4] = key_map_p1[keybroad::FIRE];

		return true;
	}
	else if (owner == TankProperty::TankOwner::PLAYER2)
	{
		key[0] = key_map_p2[keybroad::UP];
		key[1] = key_map_p2[keybroad::DOWN];
		key[2] = key_map_p2[keybroad::LEFT];
		key[3] = key_map_p2[keybroad::RIGHT];
		key[4] = key_map_p2[keybroad::FIRE];

		return true;
	}

	return false;
}


// 处理键盘输入
static void handlePlayerInputToTank(PlayerInput& input, TankProperty& tank, Controller& controller)
{
	int key[5];
	if (!Get_keymap(tank.owner, key))
		return;

	input.setInput(PlayerInput::FORWARD, GetAsyncKeyState(key[0]) & 0x8000);
	input.setInput(PlayerInput::BACKWARD, GetAsyncKeyState(key[1]) & 0x8000);
	input.setInput(PlayerInput::LEFT, GetAsyncKeyState(key[2]) & 0x8000);
	input.setInput(PlayerInput::RIGHT, GetAsyncKeyState(key[3]) & 0x8000);
	input.setInput(PlayerInput::FIRE, GetAsyncKeyState(key[4]) & 0x8000);

	bool pressFORWARD = input.isPressed(PlayerInput::FORWARD);
	bool pressBACKWARD = input.isPressed(PlayerInput::BACKWARD);
	bool pressLEFT = input.isPressed(PlayerInput::LEFT);
	bool pressRIGHT = input.isPressed(PlayerInput::RIGHT);
	bool pressFIRE = input.isPressed(PlayerInput::FIRE);

	if (pressFORWARD == pressBACKWARD)
		controller.setWantToMove(Controller::MoveDirection::NONE);
	else
	{
		if (pressFORWARD) 		controller.setWantToMove(Controller::MoveDirection::FORWARD);
		else if (pressBACKWARD) controller.setWantToMove(Controller::MoveDirection::BACKWARD);
	}

	if (pressLEFT == pressRIGHT)
		controller.setWantToRotate(Controller::RotationDirection::NONE);
	else
	{
		if (pressLEFT) 		controller.setWantToRotate(Controller::RotationDirection::LEFT);
		else if (pressRIGHT) controller.setWantToRotate(Controller::RotationDirection::RIGHT);
	}

	controller.setWantToFire(pressFIRE);
}

void LocalInputSystem::preUpdate(float deltaTime)
{
	auto& world = getWorld();
	std::vector<Entity> entities = world.getEntitiesWith<PlayerInput, TankProperty, Controller>();
	for (auto& entity : entities)
	{
		auto& playerInput = world.getComponent<PlayerInput>(entity);
		auto& tankProperty = world.getComponent<TankProperty>(entity);
		auto& movement = world.getComponent<Controller>(entity);

		if (tankProperty.owner == TankProperty::TankOwner::PLAYER1 || tankProperty.owner == TankProperty::TankOwner::PLAYER2)
			handlePlayerInputToTank(playerInput, tankProperty, movement);
	}
}
