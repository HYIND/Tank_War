#include "stdafx.h"
#include "ECS/Systems/ClientInputSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/PlayerInput.h"
#include "ECS/Components/TankProperty.h"
#include "ECS/Components/Controller.h"
#include "Helper/keymap.h"
#include "command.h"
#include "Manager/ConnectManager.h"
#include "Manager/UserInfoManager.h"

void Get_keymap(int key[5])
{
	key[0] = key_map_p1[keybroad::UP];
	key[1] = key_map_p1[keybroad::DOWN];
	key[2] = key_map_p1[keybroad::LEFT];
	key[3] = key_map_p1[keybroad::RIGHT];
	key[4] = key_map_p1[keybroad::FIRE];
}


// 处理键盘输入
static void handlePlayerInputToTank(PlayerInput& input, TankProperty& tank, Controller& controller)
{
	int key[5];
	Get_keymap(key);

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

void sendInputStateToService(PlayerInput& input)
{
	json js;
	js["command"] = GameServiceCommand::GameService_InputToGame;

	json js_inputstate;

	bool pressFORWARD = input.isPressed(PlayerInput::FORWARD);
	bool pressBACKWARD = input.isPressed(PlayerInput::BACKWARD);
	bool pressLEFT = input.isPressed(PlayerInput::LEFT);
	bool pressRIGHT = input.isPressed(PlayerInput::RIGHT);
	bool pressFIRE = input.isPressed(PlayerInput::FIRE);

	js_inputstate["forward"] = input.isPressed(PlayerInput::FORWARD) ? 1 : 0;
	js_inputstate["backword"] = input.isPressed(PlayerInput::BACKWARD) ? 1 : 0;
	js_inputstate["left"] = input.isPressed(PlayerInput::LEFT) ? 1 : 0;
	js_inputstate["right"] = input.isPressed(PlayerInput::RIGHT) ? 1 : 0;
	js_inputstate["fire"] = input.isPressed(PlayerInput::FIRE) ? 1 : 0;

	js["inputstate"] = js_inputstate;

	CONNECTMANAGER->GameSend(js);
}

void ClientInputSystem::update(float deltaTime)
{
}

void ClientInputSystem::preUpdate(float fixedDeltaTime)
{
	auto& world = getWorld();
	std::vector<Entity> entities = world.getEntitiesWith<PlayerInput, TankProperty, Controller>();
	for (auto& entity : entities)
	{
		auto& playerInput = world.getComponent<PlayerInput>(entity);
		auto& tankProperty = world.getComponent<TankProperty>(entity);
		auto& movement = world.getComponent<Controller>(entity);

		if (tankProperty.playerId == UserInfoManager::Instance()->usertoken())
		{
			handlePlayerInputToTank(playerInput, tankProperty, movement);
			sendInputStateToService(playerInput);
		}
	}
}
