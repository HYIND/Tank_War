#pragma once

#include "ECS/Core/Entity.h"
#include "Helper/math2d.h"
#include "ECS/Core/IComponent.h"
#include "GameDataDef.h"

struct BulletProperty :public IComponent
{
	int bounceCount;        // 已反弹次数
	int bulletDamage;		// 伤害
	int maxBounces;         // 最大反弹次数

	int radius;

	Entity owner;           // 发射者实体ID

	PlayerID ownerPlayerId;

	BulletProperty() {}
	BulletProperty(Entity owner, int radius, int bulletDamage)
		: bounceCount(0), bulletDamage(bulletDamage), maxBounces(5), owner(owner), radius(radius) {
	}
	BulletProperty(PlayerID ownerplayerid, int radius, int bulletDamage)
		: bounceCount(0), bulletDamage(bulletDamage), maxBounces(5), ownerPlayerId(ownerplayerid), radius(radius) {
	}
	BulletProperty(Entity owner, PlayerID ownerplayerid, int radius, int bulletDamage)
		: bounceCount(0), bulletDamage(bulletDamage), maxBounces(5), owner(owner), ownerPlayerId(ownerplayerid), radius(radius) {
	}
};