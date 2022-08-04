#include "Render.h"

extern HDC hdc;

DWORD WINAPI Render_Thread()
{
	int hr;
	timeBeginPeriod(1);
	timeInOneFps = 1.0 / Fps; // 每秒60帧，则1帧就是约16毫秒
	PAINTSTRUCT ps;
	hdc = BeginPaint(_hwnd, &ps);
	RECT rect;
	GetClientRect(_hwnd, &rect);

	_LARGE_INTEGER time_now;
	_LARGE_INTEGER time_last;
	double dqFreq;		//计时器频率
	LARGE_INTEGER f;	//计时器频率
	QueryPerformanceFrequency(&f);
	dqFreq = (double)f.QuadPart;
	QueryPerformanceCounter(&time_last);//获取计时器跳数
	int i = 0;
	while (true)
	{
		QueryPerformanceCounter(&time_now);
		while ((time_now.QuadPart - time_last.QuadPart) / dqFreq < timeInOneFps)
		{
			QueryPerformanceCounter(&time_now);
			i = (timeInOneFps - (time_now.QuadPart - time_last.QuadPart) / dqFreq) * 1000;
			if (i > 0)
				Sleep(i);
		}
		time_last = time_now;
		{
			pRenderTarget->BeginDraw();
			pRenderTarget->Clear(ColorF(1, 1, 1, 1));
			if (status != STATUS::Game_Status) {
				pRenderTarget->DrawBitmap(OP_pBitmap, D2D1::RectF(0, 0, rect.right, rect.bottom));
				//if (FAILED(hr))
				//{
				//	MessageBox(NULL, L"Draw failed!", L"Error", 0);
				//}
			}
			if (status == STATUS::Hall_Status || status == STATUS::Room_Status || (status == STATUS::Game_Status && isonline_game))
			{
				wstring ws = to_wstring(delay) + L"ms";
				const wchar_t* delay_ch = ws.c_str();
				pRenderTarget->DrawText(
					delay_ch,
					wcslen(delay_ch),
					pPing_Format,
					DelayRect,
					pMain_Brush
				);
			}
			if (status == STATUS::Game_Status)
			{
				Cur_Game->Draw();
			}
			if (CurScene)
				CurScene->DrawScene();
			hr = pRenderTarget->EndDraw();
		}
	}
	EndPaint(_hwnd, &ps);
	return 0;
}