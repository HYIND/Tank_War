#pragma once

#include "ECS/Core/Entity.h"
#include "ECS/Core/World.h"
#include "ECS/Components/BulletProperty.h"
#include "GameDataDef.h"

class BulletFactory {
public:
	static Entity CreateServerBullet(World& world, Entity owner, PlayerID playerid,int bulletDamage, float bulletSpeed,
		float x, float y, int radius,
		float rotation
	);
};