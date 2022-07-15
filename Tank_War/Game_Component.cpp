#include "Game_Component.h"
#include "D2D.h"

ID2D1Bitmap* brick_wall_pBitmap;
ID2D1Bitmap* iron_wall_pBitmap;

void Game_Component::Draw()
{
	int Reloc1 = this->locationX - (this->width) / 2;
	int Reloc2 = this->locationY - (this->height) / 2;
	pRenderTarget->DrawBitmap(Bitmap, D2D1::RectF(Reloc1, Reloc2, Reloc1 + width, Reloc2 + height));
}