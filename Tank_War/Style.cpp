#include "Style.h"

map<TankStyle, Style_info*> Tank_Style_info;
map<BulletStyle, Style_info*> Bullet_Style_info;

ID2D1Bitmap* DefTank_pBitmap;
extern HINSTANCE hInst;/* 以下为外部声明 */

void Init_Style_Resource()
{
	HRESULT hr = S_OK;
	hr = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(TANK_PNG), &DefTank_pBitmap);
}

void Init_Style()
{
	Init_Style_Resource();
	Tank_Style_info[TankStyle::DEFAULT] = new Style_info(60, 60, DefTank_pBitmap);
}