#pragma once
//d2d及位图 头文件
//#include <d2d1.h>
//#include <dwrite.h>
//#include <wincodec.h>
//#include <d2d1helper.h>
//#pragma comment(lib, "d2d1.lib")
//#pragma comment(lib,"Dwrite.lib")
//
////D2D释放资源
//#define SafeRelease(P) if(P){P->Release() ; P = NULL ;}
//
//#include <vector>
//#include <string>

#include "header.h"

#define ReturnInEndGame 5601

using namespace std;
using namespace D2D1;

class Scene;
class D2D_Bitmap;
class D2D_Text;
class D2D_Button;

extern ID2D1Factory* pD2DFactory;
extern ID2D1HwndRenderTarget* pRenderTarget;
extern IWICImagingFactory* pIWICFactory;
extern IDWriteFactory* pIDWriteFactory;

extern ID2D1SolidColorBrush* pDefaultBrush;
extern ID2D1SolidColorBrush* pClickBrush;

extern IDWriteTextFormat* pTextFormat;
extern Scene* CurScene;

extern Scene* SMain;
extern Scene* SHall;
extern Scene* SOption;
extern Scene* SRoom;
extern Scene* SPause;
extern Scene* SWinGame;
extern Scene* SFailGame;

extern int MoveX, MoveY, ClickX, ClickY;

//D2D 贴图
class D2D_Bitmap
{
public:
	int Bitmap_location1 = 0;
	int Bitmap_location2 = 0;
	int Bitmap_location3 = 0;
	int Bitmap_location4 = 0;

	D2D_Button* pButton = NULL;

	ID2D1Bitmap* pBitmap = NULL;
	D2D_Bitmap(int loc1, int loc2, int loc3, int loc4, ID2D1Bitmap* pBitmap = NULL) :
		Bitmap_location1(loc1), Bitmap_location2(loc2), Bitmap_location3(loc3), Bitmap_location4(loc4), pBitmap(pBitmap) {}
};

//D2D 文本
class D2D_Text
{
public:
	int Text_location1 = 0;
	int Text_location2 = 0;
	int Text_location3 = 0;
	int Text_location4 = 0;
	wstring str;
	ID2D1SolidColorBrush* pDefaultBrush = NULL;
	ID2D1SolidColorBrush* pClickBrush = NULL;
	IDWriteTextFormat* pTextFormat = NULL;

	D2D_Button* pButton = NULL;

	D2D_Text(int loc1, int loc2, int loc3, int loc4, const wchar_t* pwch, ID2D1SolidColorBrush* pDefaultBrush, ID2D1SolidColorBrush* pClickBrush, IDWriteTextFormat* pTextFormat) :
		Text_location1(loc1), Text_location2(loc2), Text_location3(loc3), Text_location4(loc4),
		str(pwch), pDefaultBrush(pDefaultBrush), pClickBrush(pClickBrush), pTextFormat(pTextFormat) {}
};

// D2D 按钮，绑定文本或贴图
class D2D_Button
{
public:
	int Button_location1 = 0;
	int Button_location2 = 0;
	int Button_location3 = 0;
	int Button_location4 = 0;
	int id = 0;
	D2D_Bitmap* Bitmap = NULL;
	D2D_Text* Text = NULL;
	D2D_Button(int loc1, int loc2, int loc3, int loc4, int id) :
		Button_location1(loc1), Button_location2(loc2), Button_location3(loc3), Button_location4(loc4), id(id) {}
	D2D_Button(int loc1, int loc2, int loc3, int loc4, int id, D2D_Bitmap* Bitmap) :
		Button_location1(loc1), Button_location2(loc2), Button_location3(loc3), Button_location4(loc4), id(id), Bitmap(Bitmap) {
		Bitmap->pButton = this;
	}
	D2D_Button(int loc1, int loc2, int loc3, int loc4, int id, D2D_Text* Text) :
		Button_location1(loc1), Button_location2(loc2), Button_location3(loc3), Button_location4(loc4), id(id), Text(Text) {
		Text->pButton = this;
	}
};

//场景类
class Scene
{
public:
	vector<D2D_Button*> Button_list;
	vector<D2D_Bitmap*> Bitmap_list;
	vector<D2D_Text*> Text_list;

	//两个用来监测选中变化的指针
	D2D_Text* Text_changed = NULL;
	D2D_Bitmap* Bitmap_changed = NULL;

	Scene() {};
	//初始化
	Scene(ID2D1Factory* pD2DFactory, ID2D1HwndRenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, IDWriteFactory* pIDWriteFactory) :
		pD2DFactory(pD2DFactory), pRenderTarget(pRenderTarget), pIWICFactory(pIWICFactory), pIDWriteFactory(pIDWriteFactory) {};

	// 添加位图
	D2D_Bitmap* Loadbitmap(int loc1, int loc2, int loc3, int loc4, LPCTSTR pszResource);
	// 添加文本
	D2D_Text* LoadText(int loc1, int loc2, int loc3, int loc4, const wchar_t* pwch, ID2D1SolidColorBrush* pDefaultBrush = ::pDefaultBrush, ID2D1SolidColorBrush* pClickBrush = ::pClickBrush, IDWriteTextFormat* pTextFormat = ::pTextFormat);
	// 添加按钮
	D2D_Button* LoadButton(int loc1, int loc2, int loc3, int loc4, int id);
	D2D_Button* LoadButton(int loc1, int loc2, int loc3, int loc4, int id, D2D_Bitmap* Bitmap);
	D2D_Button* LoadButton(int loc1, int loc2, int loc3, int loc4, int id, D2D_Text* Text);

	// 绘制函数
	void DrawScene();
	// 鼠标移动消息反馈函数
	void Move();
	// 点击消息反馈函数
	void Click();
private:
	ID2D1Factory* pD2DFactory = NULL; // Direct2D factory
	ID2D1HwndRenderTarget* pRenderTarget = NULL;
	IWICImagingFactory* pIWICFactory;
	IDWriteFactory* pIDWriteFactory = NULL;
};

void InitScene(ID2D1Factory*& pD2DFactory, ID2D1HwndRenderTarget*& pRenderTarget, IWICImagingFactory*& pIWICFactory, IDWriteFactory*& pDWriteFactory);
void Load_D2DResource();