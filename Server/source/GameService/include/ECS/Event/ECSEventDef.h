#pragma once

#include "ECS/Components/AllComponent.h"
#include "ECS/Core/Entity.h"
#include "Helper/math2d.h"
#include <optional>


struct EntityDestroyedEvent
{
	Entity entity;

	EntityDestroyedEvent(Entity entity) :entity(entity) {}
};

struct BulletDestroyedEvent
{
	enum class DestroyReason {
		HitTarget,     // 击中目标
		Timeout,       // 超时
		MaxBound       // 达到反弹上限
	};

	Entity bullet;
	Entity shooter;
	DestroyReason reason;  // 销毁原因
	Pos2 position;		   // 销毁位置
};

struct TankDestroyedEvent
{
	Entity tank;
	std::optional<Entity> killer;  // 击杀者
	Pos2 position;
};

struct DamageEvent
{
	Entity target;
	Entity source;       // 击杀者
	int damage;
};

struct PhysicsCollisionEvent
{
	Entity entityA;
	Entity entityB;
	Pos2 point;
	Vec2 normal;
};

struct HealthChangedEvent
{
	Entity entity;
	int currentHealth;
	int maxHealth;
};

struct EntityDeathEvent
{
	enum class DeathCause {
		Unknown,
		HealthDepleted,    // 血量耗尽
	};

	Entity entity;
	DeathCause cause = DeathCause::Unknown;
	std::optional<Entity> killer;
	Pos2 deathPosition;
};

struct WallDestroyedEvent
{
	Entity wall;
	std::optional<Entity> killer;  // 击杀者
	Pos2 position;
};

struct PropPickupEvent
{
	Entity prop;
	Entity picker;
	PropProperty::PropType propType;
};