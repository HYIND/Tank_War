#pragma once

#include "ECS/Components/PlayerInput.h"
#include "ECS/Components/TankProperty.h"
#include "ECS/Components/Controller.h"
#include "ECS/Core/System.h"
#include "Helper/TripleBuffer.h"
#include "GameDataDef.h"

struct InputState
{
	bool forward = false;
	bool backword = false;
	bool left = false;
	bool right = false;
	bool fire = false;

	void reset();
};

class ServerInputSystem : public System
{
public:
	ServerInputSystem();
	virtual ~ServerInputSystem();
	virtual void preUpdate(float fixedDeltaTime) override;

	void InputNewState(const PlayerID &playerId, const InputState &newstate);
	void handlePlayerInputToTank(PlayerInput &input, TankProperty &tank, Controller &controller);

private:
	std::unordered_map<PlayerID, std::shared_ptr<TripleBuffer<InputState>>> _input_tripbuffer_map;
};