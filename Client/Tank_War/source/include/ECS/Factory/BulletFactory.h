#pragma once

#include "ECS/Core/Entity.h"
#include "ECS/Core/World.h"
#include "ECS/Components/BulletProperty.h"
#include "GameDataDef.h"

class BulletFactory {
public:
	static Entity CreateLocalBullet(World& world, Entity owner, int bulletDamage, float bulletSpeed,
		float x, float y, int radius,
		float rotation
	);

	static Entity CreateClientBullet(World& world,
		SyncID syncid, Entity owner, PlayerID playerid,
		int bulletDamage, float bulletSpeed,
		float x, float y, int radius,
		float rotation
	);
};