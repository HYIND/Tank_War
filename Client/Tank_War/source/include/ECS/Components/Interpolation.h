#pragma once

#include "ECS/Core/IComponent.h"
#include "Helper/math2d.h"
#include <deque>

struct Interpolation :public IComponent {
	// 存储多个历史快照用于插值
	struct Snapshot {
		Pos2 position;
		float rotation;

		Vec2 velocity;
		float angularVelocity;

		uint64_t timestamp;      // 服务器时间
		uint64_t receiveTime;    // 客户端接收时间
	};

	std::deque<Snapshot> snapshots;

	// 当前插值状态
	Pos2 renderPosition;
	Vec2 renderVelocity;
	float renderRotation;

	// 插值时间线
	uint64_t currentRenderTime = 0;
	uint64_t interpolationDelay = 60;  // 插值延迟(ms)

	// 配置
	int maxSnapshots = 8;     // 最多存储8个快照
	bool enabled = true;

	// 调试信息
	float latestSnapshotDelay = 0.0f;
	float interpolationError = 0.0f;

	Interpolation() {}
	~Interpolation() {}
};