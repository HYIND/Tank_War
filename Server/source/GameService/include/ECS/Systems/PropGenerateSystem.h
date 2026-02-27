#pragma once

#include "ECS/Core/System.h"
#include "ECS/Core/Entity.h"
#include "Helper/math2d.h"

class DelayTask;

struct PropGenerateConfig {
	int maxProps = 8;            // 最大道具数量
	int minDelayMs = 3000;       // 最小生成延迟
	int maxDelayMs = 6000;       // 最大生成延迟
	int propWidth = 35.f;		 // 生成区域宽度
	int propHeight = 35.f;		 // 生成区域高度
	float propLifetime = 20.f;   // 道具生命周期
};

class PropGenerateSystem :public System
{
public:
	virtual void onDetach() override;
	virtual void preUpdate(float deltaTime);

private:
	void prepareGenerateProp();
	void doGenerateProp();
	bool GetAvaliablePropPosition(float& x, float& y, int width, int height);

private:
	std::shared_ptr<DelayTask> _prepareGenerateTimer;
	PropGenerateConfig _config;
};