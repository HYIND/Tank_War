#pragma once

#include "ECS/Core/IComponent.h"

struct Weapon :public IComponent
{
	int fireRate;           // 发射间隔（毫秒）
	int bulletDamage;         // 子弹伤害
	float bulletSpeed;        // 子弹速度
	int maxBullets;           // 最大同时存在的子弹数
	int currentBullets;       // 当前存在的子弹数

	float cooldown = 0;
	Weapon()
		: fireRate(500), bulletDamage(24),
		bulletSpeed(600.f), maxBullets(5), currentBullets(0) {
	}
};