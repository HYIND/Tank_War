#include "Prop.h"
#include "D2D.h"

ID2D1Bitmap* aidkit_pBitmap;

extern HINSTANCE hInst;

void Prop::Draw()
{
	int Reloc1 = this->locationX - (this->width) / 2;
	int Reloc2 = this->locationY - (this->height) / 2;
	pRenderTarget->DrawBitmap(Bitmap, D2D1::RectF(Reloc1, Reloc2, Reloc1 + width, Reloc2 + height));
}



void Init_Prop_Resource()
{
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(AID_KIT), &aidkit_pBitmap);
}