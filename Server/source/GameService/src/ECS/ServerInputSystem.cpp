#include "ECS/Systems/ServerInputSystem.h"
#include "ECS/Core/World.h"
#include "ECS/Components/PlayerInput.h"
#include "ECS/Components/TankProperty.h"
#include "ECS/Components/Controller.h"

ServerInputSystem::ServerInputSystem()
{
}

ServerInputSystem::~ServerInputSystem()
{
}

void ServerInputSystem::preUpdate(float fixedDeltaTime)
{
	auto &world = getWorld();
	std::vector<Entity> entities = world.getEntitiesWith<PlayerInput, TankProperty, Controller>();
	for (auto &entity : entities)
	{
		auto &playerInput = world.getComponent<PlayerInput>(entity);
		auto &tankProperty = world.getComponent<TankProperty>(entity);
		auto &movement = world.getComponent<Controller>(entity);

		if (tankProperty.owner != TankProperty::TankOwner::AI)
			handlePlayerInputToTank(playerInput, tankProperty, movement);
	}
}

void ServerInputSystem::InputNewState(const std::string &playerId, const InputState &newstate)
{
	if (_input_tripbuffer_map.find(playerId) == _input_tripbuffer_map.end())
	{
		auto state = std::make_shared<TripleBuffer<InputState>>();
		for (int i = 0; i < 3; i++)
			state->setInitialValue(i, InputState());
		_input_tripbuffer_map[playerId] = std::move(state);
	}

	auto &tripbuffer = _input_tripbuffer_map[playerId];
	auto &state = tripbuffer->acquireWriteBuffer();
	state = newstate;
	tripbuffer->submitWriteBuffer();
}

void ServerInputSystem::handlePlayerInputToTank(PlayerInput &input, TankProperty &tank, Controller &controller)
{

	PlayerID playerId = tank.playerId;

	if (_input_tripbuffer_map.find(playerId) == _input_tripbuffer_map.end())
		return;

	auto &state = _input_tripbuffer_map[playerId]->acquireReadBuffer();

	input.setInput(PlayerInput::FORWARD, state.forward);
	input.setInput(PlayerInput::BACKWARD, state.backword);
	input.setInput(PlayerInput::LEFT, state.left);
	input.setInput(PlayerInput::RIGHT, state.right);
	input.setInput(PlayerInput::FIRE, state.fire);

	bool pressFORWARD = input.isPressed(PlayerInput::FORWARD);
	bool pressBACKWARD = input.isPressed(PlayerInput::BACKWARD);
	bool pressLEFT = input.isPressed(PlayerInput::LEFT);
	bool pressRIGHT = input.isPressed(PlayerInput::RIGHT);
	bool pressFIRE = input.isPressed(PlayerInput::FIRE);

	if (pressFORWARD == pressBACKWARD)
		controller.setWantToMove(Controller::MoveDirection::NONE);
	else
	{
		if (pressFORWARD)
			controller.setWantToMove(Controller::MoveDirection::FORWARD);
		else if (pressBACKWARD)
			controller.setWantToMove(Controller::MoveDirection::BACKWARD);
	}

	if (pressLEFT == pressRIGHT)
		controller.setWantToRotate(Controller::RotationDirection::NONE);
	else
	{
		if (pressLEFT)
			controller.setWantToRotate(Controller::RotationDirection::LEFT);
		else if (pressRIGHT)
			controller.setWantToRotate(Controller::RotationDirection::RIGHT);
	}

	controller.setWantToFire(pressFIRE);
}
void InputState::reset()
{
	forward = false;
	backword = false;
	left = false;
	right = false;
	fire = false;
}