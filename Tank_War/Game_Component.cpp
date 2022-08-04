#include "Game_Component.h"
#include "D2D.h"

ID2D1Bitmap* brick_wall_pBitmap;
ID2D1Bitmap* iron_wall_pBitmap;

extern HINSTANCE hInst;

void Game_Component::Draw()
{
	int Reloc1 = this->locationX - (this->width) / 2;
	int Reloc2 = this->locationY - (this->height) / 2;
	pRenderTarget->DrawBitmap(Bitmap, D2D1::RectF(Reloc1, Reloc2, Reloc1 + width, Reloc2 + height));
}

void Init_Component_Resource()
{
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BRICK_WALL), &brick_wall_pBitmap);
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(IRON_WALL), &iron_wall_pBitmap);
}