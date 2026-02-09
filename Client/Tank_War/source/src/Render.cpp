#include "Render.h"
#include <thread>
#include "Scene.h"
#include "RenderEngine/D2DTools.h"

std::thread::id render_thread_id;

void Render_Thread(
	HWND hwnd,
	std::shared_ptr<Render::Renderer> render,
	std::shared_ptr<DynamicFpsController> fpscontroller,
	std::shared_ptr<TripleBuffer<std::shared_ptr<Render::RenderFrameData>>> buffermanager)
{
	render_thread_id = std::this_thread::get_id();

	render->SetRenderTarget(pRenderTarget);

	for (int i = 0; i < 3; i++)
		buffermanager->setInitialValue(i, std::make_shared<Render::RenderFrameData>());
	render->SetBuffers(buffermanager);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	RECT rect;
	GetClientRect(hwnd, &rect);

	fpscontroller->reset();

	while (true)
	{
		STATUS m_status = Get_CurScene();

		bool isrunningworld = (m_status == STATUS::LocalGame_Status || m_status == STATUS::OnlineGame_Status);

		{
			//渲染
			pRenderTarget->BeginDraw();
			pRenderTarget->Clear(ColorF(1, 1, 1, 1));

			if (isrunningworld && render)
				render->renderFrame(fpscontroller->getTimeDiffMS());

			if (_Scene::CurScene)
				_Scene::CurScene->DrawScene(fpscontroller->getTimeDiffMS());
			pRenderTarget->EndDraw();
		}


		int targetfps = isrunningworld ? fpscontroller->getGameTargetFps() : 30;
		fpscontroller->setCurTargetFps(targetfps);

		fpscontroller->run();		// 帧率动态控制
	}

	EndPaint(_hwnd, &ps);
}

bool IsRenderThread()
{
	return std::this_thread::get_id() == render_thread_id;
}

