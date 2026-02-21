#pragma once

#include "ECS/Core/System.h"
#include "RenderEngine/RenderFrameManager.h"
#include "Helper/math2d.h"

class RenderSystem :public System
{
public:
	virtual void postUpdate(float deltaTime) override;
	void pushDebugLine(const Line2& line);

private:
	void processSprite(std::shared_ptr<Render::RenderFrameData> framebuffer);
	void processGIFAnimation(std::shared_ptr<Render::RenderFrameData> framebuffer);
	void processTankVisual(std::shared_ptr<Render::RenderFrameData> framebuffer);
	void processHealthShow(std::shared_ptr<Render::RenderFrameData> framebuffer);
	void processDebugLines(std::shared_ptr<Render::RenderFrameData> framebuffer);

private:
	std::vector<Line2> _DebugLines;
};