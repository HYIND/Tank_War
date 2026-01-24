#include "Render.h"
#include "ResourceManager.h"

extern HDC hdc;

DWORD WINAPI Render_Thread()
{
	int hr;
	timeBeginPeriod(1);
	timeInOneFps = 1000 / Fps; // 每秒60帧，则1帧就是约16毫秒
	PAINTSTRUCT ps;
	hdc = BeginPaint(_hwnd, &ps);
	RECT rect;
	GetClientRect(_hwnd, &rect);

	int frame_count = 0;

	_LARGE_INTEGER time_now;	//当前帧渲染时间戳
	_LARGE_INTEGER time_last;	//上一帧渲染时间戳
	_LARGE_INTEGER time_last_second;	//每隔一秒更新一次，提供秒级平均帧率控制
	time_now.QuadPart = 0;
	time_last.QuadPart = 0;
	time_last_second.QuadPart = 0;

	int sleep_time = timeInOneFps / 2;     //实际睡眠时间，受实时的帧间隔和单位秒内的帧率影响而动态调整
	double time_diff = 0;     //帧间渲染时间间隔，用于动态控制帧率

	double dqFreq;		//计时器频率
	LARGE_INTEGER f;	//计时器频率
	QueryPerformanceFrequency(&f);
	dqFreq = (double)f.QuadPart / 1000;

	while (true)
	{
		STATUS m_status = Get_CurScene();
		//渲染
		{
			pRenderTarget->BeginDraw();
			pRenderTarget->Clear(ColorF(1, 1, 1, 1));
			if (_Scene::CurScene)
				_Scene::CurScene->DrawScene(time_diff);
			hr = pRenderTarget->EndDraw();
		}

		double m_timeInOneFps = m_status == STATUS::Game_Status || m_status == STATUS::MapEdit ? timeInOneFps : max(33.33, timeInOneFps);
		// 帧率动态控制
		{
			QueryPerformanceCounter(&time_now);
			time_diff = (time_now.QuadPart - time_last.QuadPart) / dqFreq;
			time_last = time_now;
			frame_count++;
			if ((time_now.QuadPart - time_last_second.QuadPart) / dqFreq >= 1000.f)
			{
				time_last_second = time_now;
				if (frame_count <= Fps)
				{
					if (sleep_time > -160)
						sleep_time -= 5;
				}
				else if (sleep_time < 160)
					sleep_time += 5;

				//LOGINFO("Render , frame_count: {:#d} ", frame_count);
				frame_count = 0;
			}

			if (time_diff - m_timeInOneFps >= 1.f)
			{
				if (sleep_time > -160)
					sleep_time--;
			}
			else {
				if (sleep_time < 160)
					sleep_time++;
			}

			if (sleep_time > 0)
				Sleep(sleep_time);
		}
	}
	EndPaint(_hwnd, &ps);
	return 0;
}