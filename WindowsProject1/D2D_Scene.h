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

//3-主菜单,4-Hall,5-Room,6-Game

#define IDB_TWO     3302  
#define IDB_THREE   3303  
#define IDB_FOUR	3304  
#define IDB_FIVE	3305

#define IDB_ONE     3301  
#define IDB_SIX		3306
#define IDB_SEVEN	3307

#define IDB_REFRESH 4301
#define IDB_SEND 4302
#define IDB_ENTERROOM 4303
#define IDB_CREATEROOM 4304
#define IDB_EXITHALL 4305

#define EDIT_IN 4401
#define EDIT_HALL 4402
#define ROOM_LIST 4403
#define USER_LIST 4404

#define IDB_PAUSE 6301
#define ReturnInEndGame 6302

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

extern ID2D1SolidColorBrush* bullet_pBrush;

extern ID2D1SolidColorBrush* pHall_Brush;
extern ID2D1SolidColorBrush* pHall_ClickBrush;

extern ID2D1SolidColorBrush* pMain_Brush;
extern ID2D1SolidColorBrush* pMain_ClickBrush;

extern IDWriteTextFormat* pMain_Format;
extern IDWriteTextFormat* pHall_Format;
extern IDWriteTextFormat* pPing_Format;


extern ID2D1Bitmap* OP_pBitmap;
extern ID2D1Bitmap* TEXT_pBitmap;

extern ID2D1Bitmap* P1_CurTank_Form;
extern ID2D1Bitmap* P2_CurTank_Form;
extern ID2D1Bitmap* DefTank_pBitmap;


extern D2D1_RECT_F DelayRect;

extern Scene* CurScene;

extern Scene* SMain;
extern Scene* SHall;
extern Scene* SOption;
extern Scene* SRoom;
extern Scene* SPause;
extern Scene* SGaming;
extern Scene* SWinGame;
extern Scene* SFailGame;


extern HWND _hwnd;
extern HINSTANCE hInst;

extern int MoveX, MoveY, ClickX, ClickY;

//D2D 贴图
class D2D_Bitmap
{
public:
	int Bitmap_location1 = 0;
	int Bitmap_location2 = 0;
	int Bitmap_location3 = 0;
	int Bitmap_location4 = 0;

	float opacity = 1.0f;

	D2D_Button* pButton = NULL;

	ID2D1Bitmap* pBitmap = NULL;
	D2D_Bitmap(int loc1, int loc2, int loc3, int loc4, ID2D1Bitmap* pBitmap = NULL, float opacity = 1.0f) :
		Bitmap_location1(loc1), Bitmap_location2(loc2), Bitmap_location3(loc3), Bitmap_location4(loc4), pBitmap(pBitmap), opacity(opacity) {}
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
	D2D_Bitmap* Loadbitmap(int loc1, int loc2, int loc3, int loc4, 
		LPCTSTR pszResource, float opicaty = 1.0f);
	D2D_Bitmap* LoadResourceBitmap(int loc1, int loc2, int loc3, int loc4,
		LPCWSTR resourceType, LPCWSTR resourceName, float opacity = 1.0f, HINSTANCE hinstance = hInst
	);
	// 添加文本
	D2D_Text* LoadText(int loc1, int loc2, int loc3, int loc4, const wchar_t* pwch, ID2D1SolidColorBrush* pDefaultBrush = ::pMain_Brush, ID2D1SolidColorBrush* pClickBrush = ::pMain_ClickBrush, IDWriteTextFormat* pTextFormat = ::pMain_Format);
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
void Load_D2DUI(RECT& rect);
void Init_D2DResource();
