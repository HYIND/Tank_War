#include "RenderEngine/Renderer.h"
#include <memory>
#include <algorithm>
#include "Helper/Tools.h"
#include "Scene.h"
#include "Manager/MapManager.h"

using namespace Render;

extern RECT _rect;
Pos2 MapPosToRenderPos(const Pos2& mapPos)
{
	Pos2 renderPos;

	// 计算地图和渲染区域的大小
	float mapWidth = static_cast<float>(MapBoundary::right - MapBoundary::left);
	float mapHeight = static_cast<float>(MapBoundary::bottom - MapBoundary::top);
	float renderWidth = static_cast<float>(_rect.right - _rect.left);
	float renderHeight = static_cast<float>(_rect.bottom - _rect.top);

	// 归一化地图坐标 (0-1 范围)
	float normalizedX = (mapPos.x - MapBoundary::left) / mapWidth;
	float normalizedY = (mapPos.y - MapBoundary::top) / mapHeight;

	// 映射到渲染坐标
	renderPos.x = _rect.left + normalizedX * renderWidth;
	renderPos.y = _rect.top + normalizedY * renderHeight;

	return renderPos;
}

Renderer::Renderer(ID2D1HwndRenderTarget* rt, RenderTripleBufferPtr buffers)
	:_redBrush(nullptr)
{
	SetRenderTarget(rt);
	SetBuffers(buffers);
}

void Renderer::SetRenderTarget(ID2D1HwndRenderTarget* rt)
{
	_renderTarget = rt;
}

void Renderer::SetBuffers(RenderTripleBufferPtr buffers)
{
	_buffers = buffers;
}

void Renderer::renderFrame(float delatTime)
{
	if (!_buffers)
		return;

	auto framedata = _buffers->acquireReadBuffer();
	auto& contexts = framedata->renderContexts;

	std::sort(contexts.begin(), contexts.end(),
		[](const std::shared_ptr<RenderContext>& a, const std::shared_ptr<RenderContext>& b)
		{
			if (!a) return false;
			if (!b) return true;

			if (a->layer != b->layer)
				return a->layer < b->layer;
			return a->internalZOrder < b->internalZOrder;
		});

	for (auto& context : contexts)
	{
		if (!context || !context->data)
			continue;

		switch (context->type)
		{
		case RenderContextType::Sprite:
		{
			processSprite(std::static_pointer_cast<SpriteRenderData>(context->data));
			break;
		}
		case RenderContextType::GIFAnimation:
		{
			processGIFAnimation(std::static_pointer_cast<GIFAnimationRenderData>(context->data));
			break;
		}
		case RenderContextType::TankVisual:
		{
			processTankVisual(std::static_pointer_cast<TankVisualRenderData>(context->data));
			break;
		}
		case RenderContextType::HealthShow:
		{
			processHealthShow(std::static_pointer_cast<HealthShowRenderData>(context->data));
			break;
		}
		default:
			break;
		}
	}
}

void Renderer::processSprite(std::shared_ptr<SpriteRenderData> data)
{
	if (!data || !data->bitmap)
		return;
	if (data->width <= 0 || data->height <= 0)
		return;
	if (data->opacity <= 0)
		return;

	float x1, y1, x2, y2;
	x1 = data->x - float(data->width / 2.f);
	y1 = data->y - float(data->height / 2.f);
	x2 = data->x + float(data->width / 2.f);
	y2 = data->y + float(data->height / 2.f);

	Pos2 pos1 = MapPosToRenderPos({ x1,y1 });
	Pos2 pos2 = MapPosToRenderPos({ x2,y2 });

	Pos2 center = MapPosToRenderPos({ data->x, data->y });

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(data->rotation, D2D1::Point2F(center.x, center.y)));
	pRenderTarget->DrawBitmap(data->bitmap,
		D2D1::RectF(pos1.x, pos1.y, pos2.x, pos2.y),
		data->opacity);
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

void Renderer::processGIFAnimation(std::shared_ptr<GIFAnimationRenderData> data)
{

	if (!data || !data->gifInfo) return;
	UINT framecount = data->gifInfo->getFrameCount();
	if (framecount <= 0 || data->giftotalTime <= 0.f) return;
	if (data->width <= 0 || data->height <= 0) return;
	int64_t currenttime = Tool::GetTimestampMilliseconds();
	if (data->loopCount > 0 && currenttime > data->startTime + data->loopCount * data->giftotalTime) return;
	if (data->opacity <= 0.f) return;

	float x1, y1, x2, y2;
	x1 = data->x - float(data->width / 2.f);
	y1 = data->y - float(data->height / 2.f);
	x2 = data->x + float(data->width / 2.f);
	y2 = data->y + float(data->height / 2.f);

	Pos2 pos1 = MapPosToRenderPos({ x1,y1 });
	Pos2 pos2 = MapPosToRenderPos({ x2,y2 });

	UINT curIndex = UINT((currenttime - data->startTime) / (data->giftotalTime / framecount)) % framecount;
	ID2D1Bitmap* bitmap = data->gifInfo->getFrame(curIndex);
	if (bitmap != nullptr)
	{
		Pos2 center = MapPosToRenderPos({ data->x, data->y });

		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(data->rotation, D2D1::Point2F(center.x, center.y)));
		pRenderTarget->DrawBitmap(bitmap,
			D2D1::RectF(pos1.x, pos1.y, pos2.x, pos2.y),
			data->opacity);
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	}
}

void Renderer::processTankVisual(std::shared_ptr<TankVisualRenderData> data)
{
	if (!data || !data->bitmap) return;
	if (data->width <= 0 || data->height <= 0) return;

	float x1, y1, x2, y2;
	x1 = data->x - float(data->width / 2.f);
	y1 = data->y - float(data->height / 2.f);
	x2 = data->x + float(data->width / 2.f);
	y2 = data->y + float(data->height / 2.f);

	Pos2 pos1 = MapPosToRenderPos({ x1,y1 });
	Pos2 pos2 = MapPosToRenderPos({ x2,y2 });

	Pos2 center = MapPosToRenderPos({ data->x, data->y });

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(data->rotation - 90.f, D2D1::Point2F(center.x, center.y)));
	pRenderTarget->DrawBitmap(data->bitmap, D2D1::RectF(pos1.x, pos1.y, pos2.x, pos2.y));
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

void Renderer::processHealthShow(std::shared_ptr<HealthShowRenderData> data)
{
	if (!data) return;

	//计算血条长度
	float HP_halfwidth = data->width / 2.f;
	float HP_halfheight = data->height / 2.f;
	float HP_len = std::max(0.f, data->percent) * data->width;

	if (!_redBrush)
		pRenderTarget->CreateSolidColorBrush(ColorF(1, 0, 0, 1), &_redBrush);

	if (_redBrush)
	{
		{

			float x1, y1, x2, y2;
			x1 = data->x - (HP_halfwidth + data->widthpadding);
			y1 = data->y - (HP_halfheight + data->heightpadding);
			x2 = data->x + (HP_halfwidth + data->widthpadding);
			y2 = data->y + (HP_halfheight + data->heightpadding);

			Pos2 pos1 = MapPosToRenderPos({ x1,y1 });
			Pos2 pos2 = MapPosToRenderPos({ x2,y2 });

			//画血条框
			pRenderTarget->DrawRectangle(
				D2D1::RectF(
					pos1.x, pos1.y,
					pos2.x, pos2.y),
				_redBrush);
		}

		{

			float x1, y1, x2, y2;
			x1 = data->x - HP_halfwidth;
			y1 = data->y - HP_halfheight;
			x2 = data->x - HP_halfwidth + HP_len;
			y2 = data->y + HP_halfheight;

			Pos2 pos1 = MapPosToRenderPos({ x1,y1 });
			Pos2 pos2 = MapPosToRenderPos({ x2,y2 });

			//画血条
			pRenderTarget->FillRectangle(
				D2D1::RectF(
					pos1.x, pos1.y,
					pos2.x, pos2.y),
				_redBrush);
		}
	}
}