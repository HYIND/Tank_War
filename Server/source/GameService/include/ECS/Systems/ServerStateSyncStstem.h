#pragma once

#include "ECS/Components/AllComponent.h"
#include "ECS/Core/System.h"
#include "Helper/TripleBuffer.h"
#include "GameDataDef.h"
#include "stdafx.h"

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

	// 序列化方法
	json toJson() const
	{
		json result;
		result["entityid"] = entityId;
		result["syncid"] = syncId;

		result["owner"] = int(owner);
		result["playerid"] = playerId;
		result["name"] = name;

		// 位置对象
		json position;
		position["x"] = posX;
		position["y"] = posY;
		result["position"] = position;

		result["rotation"] = rotation;
		result["width"] = width;
		result["height"] = height;

		// 血量对象
		json health;
		health["max"] = maxHealth;
		health["current"] = currentHealth;
		result["health"] = health;

		result["max_move_speed"] = maxMoveSpeed;
		result["max_rotation_speed"] = maxRotationSpeed;
		result["current_move_speed"] = currentMoveSpeed;
		result["current_rotation_speed"] = currentRotationSpeed;

		result["is_alive"] = isAlive;
		result["is_moving"] = isMoving;
		result["is_shooting"] = isShooting;

		return result;
	}

	// 反序列化方法
	static TankState fromJson(const json &j)
	{
		TankState state;

		state.entityId = j.value("entityid", 0);
		state.syncId = j.value("syncid", "");

		state.owner = (TankProperty::TankOwner)j.value("owner", 0);
		state.playerId = j.value("playerid", "");
		state.name = j.value("name", "");
		state.rotation = j.value("rotation", 0.0f);
		state.width = j.value("width", 0);
		state.height = j.value("height", 0);
		state.maxMoveSpeed = j.value("max_move_speed", 0.0f);
		state.maxRotationSpeed = j.value("max_rotation_speed", 0.0f);
		state.currentMoveSpeed = j.value("current_move_speed", 0.0f);
		state.currentRotationSpeed = j.value("current_rotation_speed", 0.0f);
		state.isAlive = j.value("is_alive", false);
		state.isMoving = j.value("is_moving", false);
		state.isShooting = j.value("is_shooting", false);

		// 解析嵌套对象
		if (j.contains("position") && j["position"].is_object())
		{
			const auto &pos = j["position"];
			state.posX = pos.value("x", 0.0f);
			state.posY = pos.value("y", 0.0f);
		}

		if (j.contains("health") && j["health"].is_object())
		{
			const auto &health = j["health"];
			state.maxHealth = health.value("max", 100);
			state.currentHealth = health.value("current", 100);
		}

		return state;
	}
};

struct BulletState
{
	EntityID entityId;
	SyncID syncId;

	EntityID ownerId;
	PlayerID ownerPlayerId;

	float posX, posY;
	float rotation;

	int radius;
	int bulletDamage;

	float maxMoveSpeed;
	float maxRotationSpeed;

	float currentMoveSpeed = 0.f;
	float currentRotationSpeed = 0.f;

	// 序列化方法
	json toJson() const
	{
		json result;
		result["entityid"] = entityId;
		result["syncid"] = syncId;

		result["ownerid"] = ownerId;
		result["ownerplayerid"] = ownerPlayerId;

		json position;
		position["x"] = posX;
		position["y"] = posY;
		result["position"] = position;

		result["rotation"] = rotation;

		result["radius"] = radius;
		result["bulletdamage"] = bulletDamage;

		result["max_move_speed"] = maxMoveSpeed;
		result["max_rotation_speed"] = maxRotationSpeed;
		result["current_move_speed"] = currentMoveSpeed;
		result["current_rotation_speed"] = currentRotationSpeed;

		return result;
	}

	// 反序列化方法
	static BulletState fromJson(const json &j)
	{
		BulletState state;

		state.entityId = j.value("entityid", 0);
		state.syncId = j.value("syncid", "");

		state.ownerId = j.value("owner", 0);
		state.ownerPlayerId = j.value("ownerplayerid", "");

		state.rotation = j.value("rotation", 0.0f);

		state.radius = j.value("radius", 0);
		state.bulletDamage = j.value("bulletdamage", 0);

		state.maxMoveSpeed = j.value("max_move_speed", 0.0f);
		state.maxRotationSpeed = j.value("max_rotation_speed", 0.0f);
		state.currentMoveSpeed = j.value("current_move_speed", 0.0f);
		state.currentRotationSpeed = j.value("current_rotation_speed", 0.0f);

		// 解析嵌套对象
		if (j.contains("position") && j["position"].is_object())
		{
			const auto &pos = j["position"];
			state.posX = pos.value("x", 0.0f);
			state.posY = pos.value("y", 0.0f);
		}

		return state;
	}
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

	// 序列化方法
	json toJson() const
	{
		json result;
		result["entityid"] = entityId;
		result["syncid"] = syncId;

		result["type"] = (int)type;

		json position;
		position["x"] = posX;
		position["y"] = posY;
		result["position"] = position;
		result["rotation"] = rotation;

		result["width"] = width;
		result["height"] = height;

		result["duration"] = duration;
		result["lifetime"] = lifetime;

		return result;
	}

	// 反序列化方法
	static PropState fromJson(const json &j)
	{
		PropState state;

		state.entityId = j.value("entityid", 0);
		state.syncId = j.value("syncid", "");

		state.type = (PropProperty::PropType)j.value("type", 0);

		state.rotation = j.value("rotation", 0.0f);

		state.width = j.value("width", 0);
		state.height = j.value("height", 0);

		state.duration = j.value("duration", 0.f);
		state.lifetime = j.value("lifetime", 0.f);

		// 解析嵌套对象
		if (j.contains("position") && j["position"].is_object())
		{
			const auto &pos = j["position"];
			state.posX = pos.value("x", 0.0f);
			state.posY = pos.value("y", 0.0f);
		}

		return state;
	}
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

	// 序列化方法
	json toJson() const
	{
		json result;
		result["entityid"] = entityId;
		result["syncid"] = syncId;

		result["type"] = (int)type;

		json position;
		position["x"] = posX;
		position["y"] = posY;
		result["position"] = position;
		result["rotation"] = rotation;

		result["width"] = width;
		result["height"] = height;

		result["currenthealth"] = currenthealth;
		result["maxhealth"] = maxhealth;

		return result;
	}

	// 反序列化方法
	static WallState fromJson(const json &j)
	{
		WallState state;

		state.entityId = j.value("entityid", 0);
		state.syncId = j.value("syncid", "");

		state.type = (WallProperty::WallType)j.value("type", 0);

		state.rotation = j.value("rotation", 0.0f);

		state.width = j.value("width", 0);
		state.height = j.value("height", 0);

		state.currenthealth = j.value("currenthealth", 0.f);
		state.maxhealth = j.value("maxhealth", 0.f);

		// 解析嵌套对象
		if (j.contains("position") && j["position"].is_object())
		{
			const auto &pos = j["position"];
			state.posX = pos.value("x", 0.0f);
			state.posY = pos.value("y", 0.0f);
		}

		return state;
	}
};

struct GameState
{
	std::vector<TankState> tankState;
	std::vector<BulletState> bulletState;
	std::vector<WallState> wallState;
	std::vector<PropState> propState;

	uint64_t updateTime;

	bool hasConsume = false;

	json toJson() const
	{
		json result;

		// 将坦克状态数组序列化
		json tankArray = json::array();
		for (const auto &tank : tankState)
		{
			tankArray.push_back(tank.toJson());
		}
		json bulletArray = json::array();
		for (const auto &bullet : bulletState)
		{
			bulletArray.push_back(bullet.toJson());
		}
		json wallArray = json::array();
		for (const auto &wall : wallState)
		{
			wallArray.push_back(wall.toJson());
		}
		json porpArray = json::array();
		for (const auto &prop : propState)
		{
			porpArray.push_back(prop.toJson());
		}
		result["tanks"] = tankArray;
		result["bullets"] = bulletArray;
		result["walls"] = wallArray;
		result["props"] = porpArray;
		result["timestamp"] = updateTime;

		return result;
	}

	static GameState fromJson(const json &j)
	{
		GameState state;

		if (j.contains("tanks") && j["tanks"].is_array())
		{
			for (const auto &tankJson : j["tanks"])
			{
				state.tankState.push_back(TankState::fromJson(tankJson));
			}
		}
		if (j.contains("bullets") && j["bullets"].is_array())
		{
			for (const auto &bulletJson : j["bullets"])
			{
				state.bulletState.push_back(BulletState::fromJson(bulletJson));
			}
		}
		if (j.contains("walls") && j["walls"].is_array())
		{
			for (const auto &wallJson : j["walls"])
			{
				state.wallState.push_back(WallState::fromJson(wallJson));
			}
		}
		if (j.contains("props") && j["props"].is_array())
		{
			for (const auto &propJson : j["props"])
			{
				state.propState.push_back(PropState::fromJson(propJson));
			}
		}
		state.updateTime = j.value("timestamp", 0ULL);

		return state;
	}

	void reset()
	{
		tankState.clear();
		bulletState.clear();
		wallState.clear();
		propState.clear();
		hasConsume = false;
	}
};

class ServerStateSyncStstem : public System
{
public:
	ServerStateSyncStstem();
	virtual ~ServerStateSyncStstem();
	virtual void postUpdate(float fixedDeltaTime) override;

	GameState &GetGameState();

	void SetSyncFps(float fps);
	float GetSyncFps();

private:
	void handleSyncTank(GameState &allstate, Entity entity);
	void handleSyncBullet(GameState &allstate, Entity entity);
	void handleSyncWall(GameState &allstate, Entity entity);
	void handleSyncProp(GameState &allstate, Entity entity);

private:
	std::shared_ptr<TripleBuffer<GameState>> _gamestate_tripbuffer;
	float _update_fps;

	float _deltatime_Accumulator;
	float _update_deltatime_ms;
};