#include "ECS/Event/SyncEventDef.h"

SyncEvent::SyncEvent(SyncEventType t, std::shared_ptr<ISyncEventData> d, uint64_t timestamp, uint64_t framecount)
	: type(t)
	, timestamp(timestamp)
	, framecount(framecount)
	, data(std::move(d))
{
}

json SyncEvent::toJson() const
{
	json j;
	j["type"] = static_cast<int>(type);
	j["timestamp"] = timestamp;
	j["framecount"] = framecount;
	if (data)
		j["data"] = data->toJson();
	return j;
}

SyncEvent SyncEvent::fromJson(const json& j)
{
	SyncEvent event;
	event.type = static_cast<SyncEventType>(j.value("type", 0));
	event.timestamp = j.value("timestamp", 0ULL);
	event.framecount = j.value("framecount", 0ULL);

	if (!j.contains("data") || !j["data"].is_object())
		return event;

	const auto& dataJson = j["data"];

	switch (event.type)
	{
	case SyncEventType::TANK_DESTROYED:
		event.data = std::make_shared<TankDestroyedEventData>(TankDestroyedEventData::fromJson(dataJson));
		break;
	case SyncEventType::DAMAGE:
		event.data = std::make_shared<DamageEventData>(DamageEventData::fromJson(dataJson));
		break;
	case SyncEventType::PROP_PICKUP:
		event.data = std::make_shared<PropPickupEventData>(PropPickupEventData::fromJson(dataJson));
		break;
	case SyncEventType::PICKUP_HEAL:
		event.data = std::make_shared<PickUpHealEventData>(PickUpHealEventData::fromJson(dataJson));
		break;
	case SyncEventType::WEAPON_SHOOT:
		event.data = std::make_shared<WeaponShootEventData>(WeaponShootEventData::fromJson(dataJson));
		break;
	default:
		break;
	}

	return event;
}

json TankState::toJson() const
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

TankState TankState::fromJson(const json& j)
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
		const auto& pos = j["position"];
		state.posX = pos.value("x", 0.0f);
		state.posY = pos.value("y", 0.0f);
	}

	if (j.contains("health") && j["health"].is_object())
	{
		const auto& health = j["health"];
		state.maxHealth = health.value("max", 100);
		state.currentHealth = health.value("current", 100);
	}

	return state;
}

json BulletState::toJson() const
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

	result["type"] = int(type);
	result["width"] = width;
	result["height"] = height;
	result["bulletdamage"] = bulletDamage;

	result["max_move_speed"] = maxMoveSpeed;
	result["max_rotation_speed"] = maxRotationSpeed;
	result["current_move_speed"] = currentMoveSpeed;
	result["current_rotation_speed"] = currentRotationSpeed;

	return result;
}

BulletState BulletState::fromJson(const json& j)
{
	BulletState state;

	state.entityId = j.value("entityid", 0);
	state.syncId = j.value("syncid", "");

	state.ownerId = j.value("ownerid", 0);
	state.ownerPlayerId = j.value("ownerplayerid", "");

	state.rotation = j.value("rotation", 0.0f);

	state.type = (WeaponType)j.value("type", 0);
	state.width = j.value("width", 0);
	state.height = j.value("height", 0);
	state.bulletDamage = j.value("bulletdamage", 0);

	state.maxMoveSpeed = j.value("max_move_speed", 0.0f);
	state.maxRotationSpeed = j.value("max_rotation_speed", 0.0f);
	state.currentMoveSpeed = j.value("current_move_speed", 0.0f);
	state.currentRotationSpeed = j.value("current_rotation_speed", 0.0f);

	// 解析嵌套对象
	if (j.contains("position") && j["position"].is_object())
	{
		const auto& pos = j["position"];
		state.posX = pos.value("x", 0.0f);
		state.posY = pos.value("y", 0.0f);
	}

	return state;
}

json PropState::toJson() const
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

PropState PropState::fromJson(const json& j)
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
		const auto& pos = j["position"];
		state.posX = pos.value("x", 0.0f);
		state.posY = pos.value("y", 0.0f);
	}

	return state;
}

json WallState::toJson() const
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

WallState WallState::fromJson(const json& j)
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
		const auto& pos = j["position"];
		state.posX = pos.value("x", 0.0f);
		state.posY = pos.value("y", 0.0f);
	}

	return state;
}

json GameState::toJson() const
{
	json result;

	json tankArray = json::array();
	for (const auto& tank : tankState)
	{
		tankArray.push_back(tank.toJson());
	}
	json bulletArray = json::array();
	for (const auto& bullet : bulletState)
	{
		bulletArray.push_back(bullet.toJson());
	}
	json wallArray = json::array();
	for (const auto& wall : wallState)
	{
		wallArray.push_back(wall.toJson());
	}
	json porpArray = json::array();
	for (const auto& prop : propState)
	{
		porpArray.push_back(prop.toJson());
	}
	result["tanks"] = tankArray;
	result["bullets"] = bulletArray;
	result["walls"] = wallArray;
	result["props"] = porpArray;
	result["timestamp"] = updateTime;
	result["framecount"] = framecount;

	return result;
}

GameState GameState::fromJson(const json& j)
{
	GameState state;

	if (j.contains("tanks") && j["tanks"].is_array())
	{
		for (const auto& tankJson : j["tanks"])
		{
			state.tankState.push_back(TankState::fromJson(tankJson));
		}
	}
	if (j.contains("bullets") && j["bullets"].is_array())
	{
		for (const auto& bulletJson : j["bullets"])
		{
			state.bulletState.push_back(BulletState::fromJson(bulletJson));
		}
	}
	if (j.contains("walls") && j["walls"].is_array())
	{
		for (const auto& wallJson : j["walls"])
		{
			state.wallState.push_back(WallState::fromJson(wallJson));
		}
	}
	if (j.contains("props") && j["props"].is_array())
	{
		for (const auto& propJson : j["props"])
		{
			state.propState.push_back(PropState::fromJson(propJson));
		}
	}
	state.updateTime = j.value("timestamp", 0ULL);
	state.framecount = j.value("framecount", 0ULL);

	return state;
}

void GameState::reset()
{
	tankState.clear();
	bulletState.clear();
	wallState.clear();
	propState.clear();
	hasConsume = false;
}

inline SyncEventType TankDestroyedEventData::getType() const { return SyncEventType::TANK_DESTROYED; }

inline json TankDestroyedEventData::toJson() const
{
	json j;
	j["tankid"] = tankId;
	if (killerId.has_value())
		j["killerid"] = *killerId;
	j["pos_x"] = position.x;
	j["pos_y"] = position.y;
	return j;
}

TankDestroyedEventData TankDestroyedEventData::fromJson(const json& j)
{
	TankDestroyedEventData data;
	data.tankId = j.value("tankid", "");
	if (j.contains("killerid") && j["killerid"].is_string())
	{
		SyncID id = j.value("killerid", "");
		if (!id.empty())
			data.killerId = id;
	}
	data.position.x = j.value("pos_x", 0.f);
	data.position.y = j.value("pos_y", 0.f);
	return data;
}

inline SyncEventType DamageEventData::getType() const { return SyncEventType::DAMAGE; }

inline json DamageEventData::toJson() const
{
	json j;
	j["targetid"] = targetId;
	j["sourceid"] = sourceId;
	j["damage"] = damage;
	return j;
}

DamageEventData DamageEventData::fromJson(const json& j)
{
	DamageEventData data;
	data.targetId = j.value("targetid", "");
	data.sourceId = j.value("sourceid", "");
	data.damage = j.value("damage", 0);
	return data;
}

inline SyncEventType PropPickupEventData::getType() const { return SyncEventType::PROP_PICKUP; }

inline json PropPickupEventData::toJson() const
{
	json j;
	j["propid"] = propId;
	j["pickerid"] = pickerId;
	j["proptype"] = propType;
	return j;
}

PropPickupEventData PropPickupEventData::fromJson(const json& j)
{
	PropPickupEventData data;
	data.propId = j.value("propid", "");
	data.pickerId = j.value("pickerid", "");
	data.propType = (PropProperty::PropType)j.value("proptype", 0);
	return data;
}

inline SyncEventType PickUpHealEventData::getType() const { return SyncEventType::PICKUP_HEAL; }

inline json PickUpHealEventData::toJson() const
{
	json j;
	j["pickerid"] = pickerId;
	return j;
}

PickUpHealEventData PickUpHealEventData::fromJson(const json& j)
{
	PickUpHealEventData data;
	data.pickerId = j.value("pickerid", "");
	return data;
}

inline SyncEventType WeaponShootEventData::getType() const { return SyncEventType::WEAPON_SHOOT; }

inline json WeaponShootEventData::toJson() const
{
	json j;
	j["sourceid"] = sourceId;
	return j;
}

WeaponShootEventData WeaponShootEventData::fromJson(const json& j)
{
	WeaponShootEventData data;
	data.sourceId = j.value("sourceid", "");
	return data;
}