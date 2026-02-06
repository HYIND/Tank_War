#pragma once

#include "ECS/Core/System.h"
#include "RenderEngine/RenderFrameManager.h"

class RenderSystem :public System
{
public:
	virtual void postUpdate(float deltaTime) override;

private:
	void processSprite(std::shared_ptr<Render::RenderFrameData> framebuffer);
	void processGIFAnimation(std::shared_ptr<Render::RenderFrameData> framebuffer);
	void processTankVisual(std::shared_ptr<Render::RenderFrameData> framebuffer);
	void processHealthShow(std::shared_ptr<Render::RenderFrameData> framebuffer);
};