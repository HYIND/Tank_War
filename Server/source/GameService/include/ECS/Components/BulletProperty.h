#pragma once

#include "ECS/Core/Entity.h"
#include "Helper/math2d.h"
#include "ECS/Core/IComponent.h"
#include "GameDataDef.h"
#include "Weapon.h"

struct CircleShape : public IComponent {
	float radius;

	CircleShape() {}
	CircleShape(float r) : radius(r) {}
};

struct RectShape : public IComponent {
	float width;
	float height;

	RectShape() {}
	RectShape(float w, float h) : width(w), height(h) {}
};

struct BulletCore : public IComponent {
	WeaponType type;
	Entity owner;
	int damage;

	PlayerID ownerPlayerId;

	BulletCore() {}
	BulletCore(WeaponType type, Entity owner, int damage) :type(type), owner(owner), damage(damage) {}
	BulletCore(WeaponType type, PlayerID ownerid, int damage) : type(type), ownerPlayerId(ownerid), damage(damage) {}
	BulletCore(WeaponType type, Entity owner, PlayerID ownerid, int damage) : type(type), owner(owner), ownerPlayerId(ownerid), damage(damage) {}
};

struct BounceAbility : public IComponent
{
	int bounceCount = 0;
	int maxBounces = 5;

	BounceAbility() {};
};
