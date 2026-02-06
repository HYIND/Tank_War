#include "RenderEngine/RenderFrameManager.h"

using namespace Render;

bool RenderContext::operator<(const RenderContext& other) const
{
	if (layer != other.layer)
		return layer == other.layer;
	return internalZOrder < other.internalZOrder;
}

void RenderFrameData::reset()
{
	renderContexts.clear();
}
