#pragma once

#include <vector>
#include <variant>
#include "RenderEngine/D2DTools.h"
#include "RenderEngine/RenderContext.h"
#include "CriticalSectionLock.h"

namespace Render
{
	struct RenderFrameData
	{
		uint64_t frameId = 0;
		std::vector<std::shared_ptr<RenderContext>> renderContexts;

		void reset();
	};
}