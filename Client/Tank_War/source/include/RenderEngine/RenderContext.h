#pragma once

#include "Manager/ResourceManager.h"
#include "Helper/math2d.h"

namespace Render
{
	struct BaseRenderData
	{
		float x = 0;
		float y = 0;
		float width = 0;
		float height = 0;
		float rotation = 90.f;
	};

	struct SpriteRenderData :public BaseRenderData
	{
		float opacity = 1.0f;
		ID2D1Bitmap* bitmap = nullptr;
	};

	struct GIFAnimationRenderData :public BaseRenderData
	{
		float opacity = 1.0f;

		int64_t startTime = 0;
		float giftotalTime = 1.f;
		int loopCount = 1;

		GIFINFO* gifInfo = nullptr;
	};

	struct TankVisualRenderData :public BaseRenderData
	{
		ID2D1Bitmap* bitmap = nullptr;
	};

	struct HealthShowRenderData :public BaseRenderData
	{
		float percent = 0.f;
		float heightpadding = 0.f;
		float widthpadding = 0.f;
	};

	struct DebugLineRenderData :public BaseRenderData
	{
		Line2 line;
	};

	enum class RenderContextType
	{
		DebugLine,
		Sprite,
		GIFAnimation,
		TankVisual,
		HealthShow
	};

	struct RenderContext
	{
		RenderContextType type;
		int layer = 0;
		int internalZOrder = 0;

		std::shared_ptr<BaseRenderData> data;

		bool operator<(const RenderContext& other) const;
	};
}