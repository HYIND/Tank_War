#pragma once
//d2d��λͼ ͷ�ļ�
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <d2d1helper.h>
#pragma comment(lib, "d2d1.lib")

#include <vector>

using namespace std;

//������
class Sence
{
public:
	vector<D2D_Button> Button_list;
	void Loadbitmap(pIWICFactory, pRenderTarget, Tank_Resource, &Tank_pBitmap);
private:
	ID2D1Factory* pD2DFactory = NULL; // Direct2D factory
	ID2D1HwndRenderTarget* pRenderTarget = NULL;
};

// D2D ��ť
class D2D_Button
{
	int D2D_location1;
	int D2D_location1;
	int D2D_location1;
	int D2D_location1;
};