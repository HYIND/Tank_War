#include "Style.h"

map<TankStyle, Style_info*> Tank_Style_info;
map<BulletStyle, Style_info*> Bullet_Style_info;

ID2D1Bitmap* Def_Tank_pBitmap;
ID2D1Bitmap* Def_Bullet_pBitmap;
ID2D1Bitmap* Orange_Bullet_pBitmap;
ID2D1Bitmap* Green_Bullet_pBitmap;
ID2D1Bitmap* Purple_Bullet_pBitmap;


/* 以下为外部声明 */
extern HINSTANCE hInst;

void Init_Style_Resource()
{
	HRESULT hr = S_OK;
	//hr = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"GIF", MAKEINTRESOURCE(TEST_GIF), &Def_Tank_pBitmap);
	//hr = Loadbitmap(pIWICFactory, pRenderTarget, L"C:\\Users\\H\\Desktop\\Tank_War\\x64\\Debug\\Resource\\1213578647.gif", &Def_Tank_pBitmap);

	hr = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(TANK_PNG), &Def_Tank_pBitmap);
	hr = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BULLET_PNG), &Def_Bullet_pBitmap);
	hr = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BULLET_ORANGE_PNG), &Orange_Bullet_pBitmap);
	hr = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BULLET_GREEN_PNG), &Green_Bullet_pBitmap);
	hr = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BULLET_PURPLE_PNG), &Purple_Bullet_pBitmap);
}

void Init_Style()
{
	Init_Style_Resource();

	/* Tank风格资源 */
	{
		Tank_Style_info[TankStyle::DEFAULT] = new Style_info(60, 60, 5, Def_Tank_pBitmap);
	}

	/* bullet风格资源 */
	{
		Bullet_Style_info[BulletStyle::DEFAULT] = new Style_info(30, 30, 7, Def_Bullet_pBitmap);
		Bullet_Style_info[BulletStyle::ORANGE] = new Style_info(50, 95, 7, Orange_Bullet_pBitmap);
		Bullet_Style_info[BulletStyle::GREEN] = new Style_info(50, 95, 7, Green_Bullet_pBitmap);
		Bullet_Style_info[BulletStyle::PURPLE] = new Style_info(50, 95, 7, Purple_Bullet_pBitmap);
	}
}