#pragma once

// ================ 基础组件 ================
#include "Transform.h"          // 变换（位置、旋转、缩放）
#include "RenderBase.h"         // 可渲染组件基类
#include "LifeTime.h"			// 生命周期

// ================ 渲染组件 ================
#include "TankVisual.h"			// 坦克视觉效果
#include "HealthShow.h"			// 生命值显示

// ================ 物理组件 ================
#include "Movement.h"           // 移动控制
#include "Physics.h"            // 物理碰撞
#include "BoundaryPhysisc.h"	// 游戏物理边界

// ================ 游戏实体属性 ================
#include "TankProperty.h"       // 坦克属性
#include "WallProperty.h"       // 墙体属性
#include "PropProperty.h"       // 道具属性
#include "BulletProperty.h"     // 子弹属性
#include "Weapon.h"             // 武器系统
#include "Health.h"             // 生命值

// ================ 输入与控制 ================
#include "PlayerInput.h"        // 玩家输入
#include "Controller.h"         // 控制器（意图）
#include "AIControl.h"          // AI控制

// ================ 标签组件（空结构体） ================
#include "Tags.h"               // 实体标签（Tank, Wall等）

// ================ 组件类型定义（可选） ================
namespace AllComponents {
	// 方便在工厂函数中使用类型列表
	using Types = std::tuple<
		Transform,
		RenderBase,
		Movement,
		Physics,
		TankProperty,
		TankVisual,
		WallProperty,
		PropProperty,
		BulletProperty,
		Weapon,
		Health,
		PlayerInput,
		Controller,
		AIControl,
		HealthShow,
		LifeTime,
		BoundaryPhysisc
	>;
}

// ================ 便捷函数（可选） ================
template<typename T>
constexpr const char* GetComponentName() {
	if constexpr (std::is_same_v<T, Transform>) return "Transform";
	else if constexpr (std::is_same_v<T, Movement>) return "Movement";
	else if constexpr (std::is_same_v<T, TankProperty>) return "TankProperty";
	else if constexpr (std::is_same_v<T, TankVisual>) return "TankVisual";
	else if constexpr (std::is_same_v<T, Weapon>) return "Weapon";
	else if constexpr (std::is_same_v<T, Health>) return "Health";
	else if constexpr (std::is_same_v<T, PlayerInput>) return "PlayerInput";
	else if constexpr (std::is_same_v<T, Controller>) return "Controller";
	else if constexpr (std::is_same_v<T, AIControl>) return "AIControl";
	else if constexpr (std::is_same_v<T, WallProperty>) return "WallProperty";
	else if constexpr (std::is_same_v<T, PropProperty>) return "PropProperty";
	else if constexpr (std::is_same_v<T, BulletProperty>) return "BulletProperty";
	else if constexpr (std::is_same_v<T, RenderBase>) return "RenderBase";
	else return "Unknown";
}