#pragma once

#include "header.h"
#include "D2D.h"

/*
 各场景按钮和控件的编号
 首位：3-Main,4-Hall,5-Room,6-Game,7-Option
 次位：3-按钮，4-控件
*/

#define IDB_LOCALGAME     3301
#define IDB_ENTERHALL   3302
#define IDB_OPTION	3303
#define IDB_QUITGAME	3304

#define IDB_REFRESH 4301
#define IDB_HALL_SEND 4302
#define IDB_ENTERROOM 4303
#define IDB_CREATEROOM 4304
#define IDB_EXITHALL 4305
#define HALL_EDIT_IN 4401
#define EDIT_HALL 4402
#define HALL_ROOM_LIST 4403
#define HALL_USER_LIST 4404

#define IDB_READY 5301
#define IDB_CANCELREADY 5302
#define IDB_STARTGAME	5303
#define IDB_EXITROOM	5304
#define IDB_ROOM_SEND 5305
#define ROOM_EDIT_IN 5401
#define EDIT_ROOM 5402
#define ROOM_USER_LIST 5403

#define IDB_PAUSE 6301
#define IDB_RETURN 6302
#define ReturnInEndGame 6303
#define ReturnToHall 6304

#define IDB_SETFPS_30 7301
#define IDB_SETFPS_60 7302
#define IDB_SETFPS_144 7303
#define IDB_EXITOPTION 7304
#define EDIT_EKY1_UP 7401
#define EDIT_EKY1_DOWN 7402
#define EDIT_EKY1_LEFT 7403
#define EDIT_EKY1_RIGHT 7404
#define EDIT_EKY1_FIRE 7405
#define EDIT_EKY2_UP 7406
#define EDIT_EKY2_DOWN 7407
#define EDIT_EKY2_LEFT 7408
#define EDIT_EKY2_RIGHT 7409
#define EDIT_EKY2_FIRE 7410

using namespace std;
using namespace D2D1;

//位图类，文字类、按钮类和场景基类
class D2D_Bitmap;
class D2D_Text;
class D2D_Button;
class Scene;

//场景派生类
class Scene_Option;
class Scene_Hall;
class Scene_Room;

/* 以下为内部变量的声明 */
extern ID2D1SolidColorBrush* pRed_Brush;

extern ID2D1SolidColorBrush* pHall_Brush;
extern ID2D1SolidColorBrush* pHall_ClickBrush;

extern ID2D1SolidColorBrush* pMain_Brush;
extern ID2D1SolidColorBrush* pMain_ClickBrush;

extern IDWriteTextFormat* pMain_Format;
extern IDWriteTextFormat* pHall_Format;
extern IDWriteTextFormat* pPing_Format;
extern HFONT edit_listbox_front;

extern ID2D1Bitmap* OP_pBitmap;
extern ID2D1Bitmap* TEXT_pBitmap;

extern D2D1_RECT_F DelayRect;

extern Scene* CurScene;

extern Scene* SMain;
extern Scene_Hall* SHall;
extern Scene_Option* SOption;
extern Scene_Room* SRoom_host;
extern Scene_Room* SRoom_nothost;
extern Scene* SPause;
extern Scene* SGaming_online;
extern Scene* SGaming_local;
extern Scene* SWinGame;
extern Scene* SFailGame;

extern HWND userid_in;

//extern HWND Hall;
//extern HWND Hall_room_list;
//extern HWND Hall_user_list;
//extern HWND Hall_edit_in;
//extern HWND edit_hall;

//extern HWND Room_user_list;
//extern HWND Room_edit_in;
//extern HWND edit_room;
//extern HWND Room;

//status 枚举
enum class STATUS { Main, Option, Hall_Status, Room_Status, Game_Status };
extern STATUS status;

/* 以下为外部变量的声明 */
extern HINSTANCE hInst;
extern HWND _hwnd;
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

	//int status = 0;

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
	//int status = 0;
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

	// 修改按钮
	bool ModifyButton_Location(int id, int loc1, int loc2, int loc3, int loc4, bool offset);
	bool ModifyButton_ID(int oldid, int newid);
	//修改文字
	bool ModifyText_byButton(int id, wstring newstr);
	//修改位图信息
	bool ModifyBitmap_byButton(int id, int loc1, int loc2, int loc3, int loc4, bool offset, float opcaity);

	//删除按钮 
	bool DeleteButton(int id);

	// 绘制函数
	virtual void DrawScene();
	// 鼠标移动消息反馈函数
	void Move();
	// 点击消息反馈函数
	void Click();
protected:
	ID2D1Factory* pD2DFactory = NULL; // Direct2D factory
	ID2D1HwndRenderTarget* pRenderTarget = NULL;
	IWICImagingFactory* pIWICFactory;
	IDWriteFactory* pIDWriteFactory = NULL;
};

class Scene_Option :public Scene
{
public:
	HWND key1_UP;
	HWND key1_DOWN;
	HWND key1_LEFT;
	HWND key1_RIGHT;
	HWND key1_FIRE;

	HWND key2_UP;
	HWND key2_DOWN;
	HWND key2_LEFT;
	HWND key2_RIGHT;
	HWND key2_FIRE;

	vector<HWND*> HWND_Messager;
	map<HWND, enum class keybroad> key_map_set1;
	map<HWND, enum class keybroad> key_map_set2;

	using Scene::Scene;
	virtual void DrawScene();
	void Get_Key();
};

class Scene_Hall :public Scene
{
public:
	using Scene::Scene;
	HWND Hall;
	HWND Hall_room_list;
	HWND Hall_user_list;
	HWND Hall_edit_in;
	HWND edit_hall;
};


class Scene_Room :public Scene
{
public:
	using Scene::Scene;
	static HWND Room_user_list;
	static HWND Room_edit_in;
	static HWND edit_room;
};

void Init_SceneResource();
