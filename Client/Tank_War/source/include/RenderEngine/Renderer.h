#pragma once

#include "RenderEngine/D2DTools.h"
#include "RenderEngine/RenderFrameManager.h"
#include "Helper/TripleBuffer.h"

namespace Render
{

	class Renderer
	{

	public:
		using RenderTripleBufferPtr = std::shared_ptr<TripleBuffer<std::shared_ptr<RenderFrameData>>>;

	public:
		Renderer(ID2D1HwndRenderTarget* rt = nullptr, RenderTripleBufferPtr sharedptr = nullptr);

		void SetRenderTarget(ID2D1HwndRenderTarget* rt);
		void SetBuffers(RenderTripleBufferPtr buffers);

		void renderFrame(float delatTime);

	private:
		void processSprite(std::shared_ptr<SpriteRenderData> data);
		void processGIFAnimation(std::shared_ptr<GIFAnimationRenderData> data);
		void processTankVisual(std::shared_ptr<TankVisualRenderData> data);
		void processHealthShow(std::shared_ptr<HealthShowRenderData> data);

	private:
		ID2D1HwndRenderTarget* _renderTarget = nullptr;
		RenderTripleBufferPtr _buffers;
		ID2D1SolidColorBrush* _redBrush;
	};
}