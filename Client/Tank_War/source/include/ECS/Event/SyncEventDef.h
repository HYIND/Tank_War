#pragma once

#include "stdafx.h"
#include "GameDataDef.h"
#include "ECS/Components/AllComponent.h"             
#include "Helper/math2d.h"

// 网络事件类型枚举
enum class SyncEventType : uint16_t
{
	// 实体相关
	TANK_DESTROYED,

	// 伤害相关
	DAMAGE,

	// 道具相关
	PROP_PICKUP,
	PICKUP_HEAL,

	// 武器相关
	WEAPON_SHOOT,

	// 预留自定义事件起始ID
	CUSTOM_EVENT_START = 1000
};

struct ISyncEventData
{
	virtual ~ISyncEventData() = default;
	virtual SyncEventType getType() const = 0;
	virtual json toJson() const = 0;
};

struct SyncEvent
{
	SyncEventType type;
	uint64_t timestamp;           // 服务器时间戳
	uint64_t framecount;
	std::shared_ptr<ISyncEventData> data;

	SyncEvent() = default;
	SyncEvent(SyncEventType t, std::shared_ptr<ISyncEventData> d, uint64_t timestamp = 0, uint64_t framecount = 0);
	json toJson() const;
	static SyncEvent fromJson(const json& j);
};

struct TankState
{
	EntityID entityId;
	SyncID syncId;

	TankProperty::TankOwner owner;
	PlayerID playerId;
	PlayerName name;

	// 位置和旋转
	float posX, posY;
	float rotation; // 坦克朝向

	// 属性
	int width, height;
	int maxHealth, currentHealth;

	float maxMoveSpeed;
	float maxRotationSpeed;

	float currentMoveSpeed = 0.f;
	float currentRotationSpeed = 0.f;

	// 状态
	bool isAlive;
	bool isMoving;
	bool isShooting;

	json toJson() const;
	static TankState fromJson(const json& j);
};

struct BulletState
{
	EntityID entityId;
	SyncID syncId;

	EntityID ownerId;
	PlayerID ownerPlayerId;

	float posX, posY;
	float rotation;

	WeaponType type;
	int width;
	int height;
	int bulletDamage;

	float maxMoveSpeed;
	float maxRotationSpeed;

	float currentMoveSpeed = 0.f;
	float currentRotationSpeed = 0.f;

	json toJson() const;
	static BulletState fromJson(const json& j);
};

struct PropState
{
	EntityID entityId;
	SyncID syncId;

	PropProperty::PropType type;

	float posX, posY;
	float rotation;

	int width, height;
	float duration;

	float lifetime;

	json toJson() const;
	static PropState fromJson(const json& j);
};

struct WallState
{
	EntityID entityId;
	SyncID syncId;

	WallProperty::WallType type;

	float posX, posY;
	float rotation;

	int width, height;

	int currenthealth;
	int maxhealth;

	json toJson() const;
	static WallState fromJson(const json& j);
};

struct GameState
{
	std::vector<TankState> tankState;
	std::vector<BulletState> bulletState;
	std::vector<WallState> wallState;
	std::vector<PropState> propState;

	uint64_t updateTime;
	uint64_t framecount;

	bool hasConsume = false;

	json toJson() const;
	static GameState fromJson(const json& j);

	void reset();
};

// 坦克销毁事件
struct TankDestroyedEventData : public ISyncEventData
{
	SyncID tankId;
	std::optional<SyncID> killerId;  // 可能为空
	Pos2 position;

	virtual SyncEventType getType() const override;
	virtual json toJson() const override;
	static TankDestroyedEventData fromJson(const json& j);
};

// 伤害事件
struct DamageEventData : public ISyncEventData
{
	SyncID targetId;
	SyncID sourceId;
	int damage;

	virtual SyncEventType getType() const override;
	virtual json toJson() const override;
	static DamageEventData fromJson(const json& j);
};

struct PropPickupEventData : public ISyncEventData
{
	SyncID propId;
	SyncID pickerId;
	PropProperty::PropType propType;

	virtual SyncEventType getType() const override;
	virtual json toJson() const override;
	static PropPickupEventData fromJson(const json& j);
};

struct PickUpHealEventData : public ISyncEventData
{
	SyncID pickerId;

	virtual SyncEventType getType() const override;
	virtual json toJson() const override;
	static PickUpHealEventData fromJson(const json& j);
};

// 武器射击事件
struct WeaponShootEventData : public ISyncEventData
{
	SyncID sourceId;

	virtual SyncEventType getType() const override;
	virtual json toJson() const override;
	static WeaponShootEventData fromJson(const json& j);
};
