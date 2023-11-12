#pragma once

#include "header.h"


extern ID2D1Factory* pD2DFactory;
extern ID2D1HwndRenderTarget* pRenderTarget;
extern IWICImagingFactory* pIWICFactory;
extern IDWriteFactory* pIDWriteFactory;

extern double Fps;
extern double timeInOneFps;

void Set_Fps(int Fps_in);

ID2D1Bitmap* LoadResourceBitmap(HINSTANCE hinstance,
	IWICImagingFactory* pIWICFactory, ID2D1RenderTarget* pRenderTarget,
	LPCWSTR resourceType, LPCWSTR resourceName);

ID2D1Bitmap* Loadbitmap(IWICImagingFactory* pIWICFactory, ID2D1RenderTarget* pRenderTarget,
	LPCTSTR pszResource);

vector<ID2D1Bitmap*> LoadResourceGIF(
	HINSTANCE hinstance,
	IWICImagingFactory* pIWICFactory, ID2D1RenderTarget* pRenderTarget,
	LPCWSTR resourceType, LPCWSTR resourceName);