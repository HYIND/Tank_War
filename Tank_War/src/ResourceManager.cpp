#include "ResourceManager.h"
#include "header.h"
#include "Scene.h"

extern HINSTANCE hInst;
extern HWND _hwnd;

GIFINFO::GIFINFO(double mstime, const std::vector<ID2D1Bitmap*>& VecBP) {
	defaultTime = mstime;
	_VecBP = VecBP;
}
double GIFINFO::getDefaultMsTime() {
	return defaultTime;
}
std::vector<ID2D1Bitmap*>& GIFINFO::getVecBP() {
	return _VecBP;
}

ResourceManager::ResourceManager() {}
ResourceManager* ResourceManager::Instance() {
	static ResourceManager* m_Instance = new ResourceManager();
	return m_Instance;
}

bool Need() {
	static bool init = false;
	if (init)return true;

	RECT rect;
	GetClientRect(_hwnd, &rect);
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
	pD2DFactory->CreateHwndRenderTarget(
		RenderTargetProperties(),
		HwndRenderTargetProperties(_hwnd, SizeU(rect.right - rect.left, rect.bottom - rect.top)),
		&pRenderTarget
	);
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pIDWriteFactory)
	);

	init = true;
	return init;
}

bool ResourceManager::InitResource() {
	Need();

	ID2D1Bitmap* textBK = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG));
	ID2D1Bitmap* returnBP = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(RETURN_PNG));
	ID2D1Bitmap* pauseBP = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(PAUSE_PNG));
	ID2D1Bitmap* winBP = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(WIN_PNG));
	ID2D1Bitmap* failBP = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(FAIL_PNG));


	ID2D1Bitmap* opBK = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(OPBK_PNG));
	ID2D1Bitmap* brick_wall_pBitmap = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BRICK_WALL));
	ID2D1Bitmap* iron_wall_pBitmap = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(IRON_WALL));
	ID2D1Bitmap* sand_BK = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"JPG", MAKEINTRESOURCE(BK_SAND));
	ID2D1Bitmap* aidkit_pBitmap = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(AID_KIT));
	ID2D1Bitmap* Def_Tank_pBitmap = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(TANK_PNG));
	ID2D1Bitmap* Def_Bullet_pBitmap = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BULLET_PNG));
	ID2D1Bitmap* Orange_Bullet_pBitmap = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BULLET_ORANGE_PNG));
	ID2D1Bitmap* Green_Bullet_pBitmap = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BULLET_GREEN_PNG));
	ID2D1Bitmap* Purple_Bullet_pBitmap = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BULLET_PURPLE_PNG));

	GIFINFO* Explosion_GIF = new GIFINFO(500, LoadResourceGIF(hInst, pIWICFactory, pRenderTarget, L"GIF", MAKEINTRESOURCE(EXPLOISION_GIF)));

	BitMapRes[ResName::textBK] = textBK;
	BitMapRes[ResName::returnBP] = returnBP;
	BitMapRes[ResName::pauseBP] = pauseBP;
	BitMapRes[ResName::winBP] = winBP;
	BitMapRes[ResName::failBP] = failBP;
	BitMapRes[ResName::opBK] = opBK;
	BitMapRes[ResName::brickWall] = brick_wall_pBitmap;
	BitMapRes[ResName::ironWall] = iron_wall_pBitmap;
	BitMapRes[ResName::sandBK] = sand_BK;
	BitMapRes[ResName::aidKit] = aidkit_pBitmap;
	BitMapRes[ResName::defTank] = Def_Tank_pBitmap;
	BitMapRes[ResName::defBullet] = Def_Bullet_pBitmap;
	BitMapRes[ResName::orangeBullet] = Orange_Bullet_pBitmap;
	BitMapRes[ResName::greenBullet] = Green_Bullet_pBitmap;
	BitMapRes[ResName::purpleBullet] = Purple_Bullet_pBitmap;

	GIFRes[ResName::explosionGIF] = Explosion_GIF;
	return true;
}

ID2D1Bitmap* ResourceManager::GetBitMapRes(const string& name) {
	auto it = BitMapRes.find(name);
	if (it == BitMapRes.end())
		return nullptr;
	return it->second;
}

GIFINFO* ResourceManager::GetGIFRes(const string& name) {
	auto it = GIFRes.find(name);
	if (it == GIFRes.end())
		return {};
	return it->second;
}