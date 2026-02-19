#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components/AllComponent.h"
#include "ECS/Core/World.h"
#include "Manager/RenderManager.h"

using namespace Render;

void RenderSystem::postUpdate(float deltaTime)
{
	auto bufferframe = RenderManager::Instance()->getBufferManager()->acquireWriteBuffer();

	processSprite(bufferframe);
	processGIFAnimation(bufferframe);
	processTankVisual(bufferframe);
	processHealthShow(bufferframe);

	RenderManager::Instance()->getBufferManager()->submitWriteBuffer();
}

void RenderSystem::processSprite(std::shared_ptr<RenderFrameData> framebuffer)
{
	auto entities = m_world->getEntitiesWith<Transform, Sprite>();

	for (auto entity : entities)
	{
		auto& transform = entity.getComponent<Transform>();
		auto& sprite = entity.getComponent<Sprite>();

		if (!sprite.bitmap) continue;
		if (sprite.opacity <= 0) continue;
		if (sprite.width <= 0 || sprite.height <= 0) continue;

		auto renderdata = std::make_shared<SpriteRenderData>();
		renderdata->x = transform.position.x - sprite.offset.x;
		renderdata->y = transform.position.y - sprite.offset.y;
		renderdata->rotation = transform.rotation;
		renderdata->width = sprite.width;
		renderdata->height = sprite.height;
		renderdata->bitmap = sprite.bitmap;
		renderdata->opacity = sprite.opacity;

		auto context = std::make_shared<RenderContext>();
		context->type = RenderContextType::Sprite;
		context->layer = sprite.layer;
		context->internalZOrder = sprite.internalZOrder;
		context->data = renderdata;

		framebuffer->renderContexts.emplace_back(context);
	}
}

void RenderSystem::processGIFAnimation(std::shared_ptr<RenderFrameData> framebuffer)
{
	auto entities = m_world->getEntitiesWith<Transform, GIFAnimator>();

	for (auto entity : entities)
	{
		auto& transform = entity.getComponent<Transform>();
		auto& gifanimator = entity.getComponent<GIFAnimator>();

		if (!gifanimator.gifInfo)continue;
		UINT framecount = gifanimator.gifInfo->getFrameCount();
		if (framecount <= 0 || gifanimator.giftotalTime <= 0.f) continue;
		if (gifanimator.width <= 0 || gifanimator.height <= 0) continue;
		int64_t currenttime = Tool::GetTimestampMilliseconds();
		if (gifanimator.loopCount > 0 && currenttime > gifanimator.startTime + gifanimator.loopCount * gifanimator.giftotalTime) continue;
		if (gifanimator.opacity <= 0.f) continue;

		auto renderdata = std::make_shared<GIFAnimationRenderData>();
		renderdata->x = transform.position.x - gifanimator.offset.x;
		renderdata->y = transform.position.y - gifanimator.offset.y;
		renderdata->rotation = transform.rotation;
		renderdata->width = gifanimator.width;
		renderdata->height = gifanimator.height;
		renderdata->opacity = gifanimator.opacity;
		renderdata->gifInfo = gifanimator.gifInfo;
		renderdata->startTime = gifanimator.startTime;
		renderdata->giftotalTime = gifanimator.giftotalTime;
		renderdata->loopCount = gifanimator.loopCount;

		auto context = std::make_shared<RenderContext>();
		context->type = RenderContextType::GIFAnimation;
		context->layer = gifanimator.layer;
		context->internalZOrder = gifanimator.internalZOrder;
		context->data = renderdata;

		framebuffer->renderContexts.emplace_back(context);
	}
}

void RenderSystem::processTankVisual(std::shared_ptr<RenderFrameData> framebuffer)
{
	auto entities = m_world->getEntitiesWith<Transform, TankVisual, TankProperty>();

	for (auto entity : entities)
	{
		auto& transform = entity.getComponent<Transform>();
		auto& tankvisual = entity.getComponent<TankVisual>();
		auto& tankproperty = entity.getComponent<TankProperty>();

		if (tankvisual.visualstate == TankVisual::VisualState::NONE) continue;
		if (tankvisual.width <= 0 || tankvisual.height <= 0) continue;

		auto renderdata = std::make_shared <TankVisualRenderData>();
		renderdata->x = transform.position.x;
		renderdata->y = transform.position.y;
		renderdata->width = tankvisual.width;
		renderdata->height = tankvisual.height;
		renderdata->rotation = transform.rotation;

		if (tankvisual.visualstate == TankVisual::VisualState::BASIC)
		{
			if (tankproperty.owner == TankProperty::TankOwner::PLAYER1)
				renderdata->bitmap = ResFactory->GetBitMapRes(ResName::redTank);			
			else if (tankproperty.owner == TankProperty::TankOwner::PLAYER2)
				renderdata->bitmap = ResFactory->GetBitMapRes(ResName::blueTank);			
			else if (tankproperty.owner == TankProperty::TankOwner::AI)
				renderdata->bitmap = ResFactory->GetBitMapRes(ResName::greenTank);
		}

		if (!renderdata->bitmap)
			renderdata->bitmap = ResFactory->GetBitMapRes(ResName::defTank);

		if (!renderdata->bitmap)
			continue;

		auto context = std::make_shared<RenderContext>();
		context->type = RenderContextType::TankVisual;
		context->layer = tankvisual.layer;
		context->internalZOrder = tankvisual.internalZOrder;
		context->data = renderdata;

		framebuffer->renderContexts.emplace_back(context);
	}
}

void RenderSystem::processHealthShow(std::shared_ptr<RenderFrameData> framebuffer)
{
	auto entities = m_world->getEntitiesWith<Transform, HealthShow, TankVisual, Health>();

	for (auto entity : entities)
	{
		auto& transform = entity.getComponent<Transform>();
		auto& healthshow = entity.getComponent<HealthShow>();
		auto& tankvisual = entity.getComponent<TankVisual>();
		auto& health = entity.getComponent<Health>();

		float HP_X = transform.position.x;
		float HP_Y = transform.position.y - tankvisual.height / 2 - 10;
		float HP_Width = healthshow.width;
		float HP_Height = healthshow.height;
		float percent = float(health.currentHealth) / float(health.maxHealth);

		auto renderdata = std::make_shared <HealthShowRenderData>();
		renderdata->x = HP_X;
		renderdata->y = HP_Y;
		renderdata->width = HP_Width;
		renderdata->height = HP_Height;
		renderdata->rotation = transform.rotation;
		renderdata->percent = percent;
		renderdata->heightpadding = 2;
		renderdata->widthpadding = 2;

		auto context = std::make_shared<RenderContext>();
		context->type = RenderContextType::HealthShow;
		context->layer = tankvisual.layer;
		context->internalZOrder = tankvisual.internalZOrder;
		context->data = renderdata;

		framebuffer->renderContexts.emplace_back(context);
	}
}