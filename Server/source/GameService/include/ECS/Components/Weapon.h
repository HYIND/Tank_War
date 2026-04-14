#pragma once

#include "ECS/Core/IComponent.h"
#include <map>

enum class WeaponType
{
	Default = 0,
	EnergyWave
};

// 武器存续类型
enum class WeaponDurationType
{
	Permanent = 0,   // 永久武器
	CountTemporary,  // 临时武器（次数）
	TimeTemporary    // 临时武器（时限）
};

struct WeaponConfig :public IComponent
{

	WeaponType type = WeaponType::Default;
	WeaponDurationType durationType = WeaponDurationType::Permanent;	//存续类型

	int fireRate;			  // 发射间隔（毫秒）
	int bulletDamage;         // 子弹伤害
	float bulletSpeed;        // 子弹速度
	int maxBullets;           // 最大同时存在的子弹数
	int currentBullets;       // 当前存在的子弹数

	float cooldown = 0;		  // 当前冷却时间

	WeaponConfig()
		: fireRate(500), bulletDamage(24),
		bulletSpeed(600.f), maxBullets(5), currentBullets(0) {
	}

	WeaponConfig(WeaponType type)
		: WeaponConfig() {
		this->type = type;
	}
};

struct WeaponContainer : public IComponent
{
	std::map<WeaponType, WeaponConfig*> weapons;

	float changeWeapon_Time = 200;		// 切换武器后可立即使用的时间间隔
	float changeWeapon_cooldown = 0;	// 切换武器后可立即使用的冷却时间计数

	WeaponConfig* tryGetWeapon(WeaponType type)
	{
		auto it = weapons.find(type);
		if (it == weapons.end())
			return nullptr;
		return it->second;
	}

	void removeWeapon(WeaponType type)
	{
		auto it = weapons.find(type);
		if (it != weapons.end())
		{
			auto config = it->second;
			if (config)
				delete config;
			weapons.erase(it);
		}
	}

	void addWeapon(WeaponConfig* config)
	{
		if (!config)
			return;

		removeWeapon(config->type);
		weapons.insert(std::pair<WeaponType, WeaponConfig*>(config->type, config));
	}

	virtual void OnRemove(const Entity& e)
	{
		for (auto it : weapons)
		{
			auto config = it.second;
			if (config)
				delete config;
		}
		weapons.clear();
	}
};