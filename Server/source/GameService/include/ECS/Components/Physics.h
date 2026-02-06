#pragma once

#include "box2d/box2d.h"
#include "Helper/math2d.h"
#include <iostream>
#include <string>
#include "ECS/Core/IComponent.h"

#define _USE_MATH_DEFINES
#include <math.h>

struct Physics :public IComponent
{
	// === 形状定义 ===
	enum class Shape { Rect, Round } shape = Shape::Rect;

	Vec2 position = { 0.0f, 0.0f };
	Vec2 size = { 1.0f, 1.0f };		// Rect: 宽高, Circle: 直径

	// === 物理类型 ===
	enum class BodyType {
		Static,    // 静态：墙、地面
		Dynamic,   // 动态：受物理影响
		Kinematic  // 运动：可移动但不受力
	};

	BodyType bodyType = BodyType::Dynamic;

	// === 物理属性 ===
	float density = 1.0f;      // 密度 kg/m²
	float friction = 0.3f;     // 摩擦 0.0-1.0
	float restitution = 0.1f;  // 弹性 0.0-1.0

	// === 高级选项 ===
	bool isSensor = false;		// 传感器模式（只检测不碰撞）
	bool fixedRotation = false; // 锁定旋转（俯视角游戏常用）
	bool isBullet = false;		// 连续碰撞检测（高速物体）


	// === 运行时数据 ===
	b2Body* body = nullptr;      // Box2D 刚体指针
	b2World* world = nullptr;    // world

	int16 groupIndex = 0;


	virtual void OnRemove() override
	{
		if (world && body)
			world->DestroyBody(body);
	}
};