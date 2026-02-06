#pragma once

#include "ECS/Core/IComponent.h"
#include "GameDataDef.h"

struct TankProperty :public IComponent
{
	enum TankOwner { PLAYER1, PLAYER2, AI };

	TankOwner owner = TankOwner::AI;

	PlayerID playerId;
	
	int width = 0;
	int height = 0;

	TankProperty() {};
	TankProperty(TankOwner type, PlayerID playerid, int width, int height)
		: owner(type), playerId(playerid), width(width), height(height) {
	}
};