#include "Scene.h"

extern HWND _hwnd;


ID2D1Factory* pD2DFactory;
ID2D1HwndRenderTarget* pRenderTarget;
IWICImagingFactory* pIWICFactory;
IDWriteFactory* pIDWriteFactory;

ID2D1SolidColorBrush* pDefaultBrush;
ID2D1SolidColorBrush* pClickBrush;

IDWriteTextFormat* pTextFormat;
Scene* CurScene;

Scene* SMain;
Scene* SHall;
Scene* SOption;
Scene* SRoom;
Scene* SPause;
Scene* SWinGame;
Scene* SFailGame;


int MoveX, MoveY, ClickX, ClickY;

D2D_Bitmap* Scene::Loadbitmap(int loc1, int loc2, int loc3, int loc4, LPCTSTR pszResource)
{
	D2D_Bitmap* Bitmap = new D2D_Bitmap(loc1, loc2, loc3, loc4);
	if (NULL == pIWICFactory)
	{
		CoInitialize(NULL);
		CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pIWICFactory)
		);
	}
	HRESULT hr = S_OK;
	IWICStream* pStream = NULL;
	IWICBitmapScaler* pScaler = NULL;
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICFormatConverter* pConverter = NULL;

	hr = pIWICFactory->CreateDecoderFromFilename(
		pszResource,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}

	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			&(Bitmap->pBitmap)
		);
	}
	if (SUCCEEDED(hr))
	{
		Bitmap_list.emplace_back(Bitmap);
	}

	SafeRelease(pDecoder);
	SafeRelease(pSource);
	SafeRelease(pStream);
	SafeRelease(pConverter);
	SafeRelease(pScaler);

	return Bitmap;
}

D2D_Text* Scene::LoadText(int loc1, int loc2, int loc3, int loc4, const wchar_t* pwch, ID2D1SolidColorBrush* pDefaultBrush, ID2D1SolidColorBrush* pClickBrush, IDWriteTextFormat* pTextFormat)
{
	D2D_Text* Text = new D2D_Text(loc1, loc2, loc3, loc4, pwch, pDefaultBrush, pClickBrush, pTextFormat);
	Text_list.emplace_back(Text);
	return Text;
}

D2D_Button* Scene::LoadButton(int loc1, int loc2, int loc3, int loc4, int id)
{
	D2D_Button* Button = new D2D_Button(loc1, loc2, loc3, loc4, id);
	Button_list.emplace_back(Button);
	return Button;
}

D2D_Button* Scene::LoadButton(int loc1, int loc2, int loc3, int loc4, int id, D2D_Bitmap* Bitmap)
{
	D2D_Button* Button = new D2D_Button(loc1, loc2, loc3, loc4, id, Bitmap);
	Button_list.emplace_back(Button);
	return Button;
}

D2D_Button* Scene::LoadButton(int loc1, int loc2, int loc3, int loc4, int id, D2D_Text* Text)
{
	D2D_Button* Button = new D2D_Button(loc1, loc2, loc3, loc4, id, Text);
	Button_list.emplace_back(Button);
	return Button;
}

void Scene::DrawScene()
{
	for (auto& v : Text_list)
	{

		D2D1_RECT_F layoutRect = RectF(v->Text_location1, v->Text_location2, v->Text_location3, v->Text_location4);
		//draw text
		pRenderTarget->DrawText(
			v->str.c_str(),
			wcslen(v->str.c_str()),
			v->pTextFormat,
			layoutRect,
			v->pDefaultBrush
		);
	}
	for (auto& v : Bitmap_list)
	{
		pRenderTarget->DrawBitmap(v->pBitmap, RectF(v->Bitmap_location1, v->Bitmap_location2, v->Bitmap_location3, v->Bitmap_location4));
	}
}

void Scene::Move()
{
	if (Text_changed)
	{
		D2D_Button* Button = Text_changed->pButton;
		if (MoveX < Button->Button_location1 || MoveX > Button->Button_location3 ||
			MoveY < Button->Button_location2 || MoveY > Button->Button_location4)
		{
			swap(Text_changed->pDefaultBrush, Text_changed->pClickBrush);
			Text_changed = NULL;
		}
		return;
	}
	else if (Bitmap_changed) {}
	else
	{
		for (auto& v : Button_list)
		{
			if (MoveX > v->Button_location1 && MoveX < v->Button_location3 &&
				MoveY > v->Button_location2 && MoveY < v->Button_location4)
			{
				if (v->Text)
				{
					swap(v->Text->pDefaultBrush, v->Text->pClickBrush);
					Text_changed = v->Text;
					return;
				}
				else {}
			}
		}
	}
}

void Scene::Click()
{
	if (Text_changed)
	{
		D2D_Button* Button = Text_changed->pButton;
		if (ClickX > Button->Button_location1 && ClickX < Button->Button_location3 &&
			ClickY > Button->Button_location2 && ClickY < Button->Button_location4)
		{
			SendMessage(_hwnd, WM_COMMAND, Button->id, (LPARAM)_hwnd);
		}
	}
	else if (Bitmap_changed) {};
}

void InitScene(ID2D1Factory*& pD2DFactory, ID2D1HwndRenderTarget*& pRenderTarget, IWICImagingFactory*& pIWICFactory, IDWriteFactory*& pDWriteFactory)
{

	try {
		::SWinGame = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SFailGame = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SMain = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SHall = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SRoom = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SOption = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
	}
	catch (exception& e)
	{
		return;
	}
}

void Load_SWinGame()
{
	::SWinGame->LoadButton(300, 300, 500, 400,
		ReturnInEndGame,
		::SWinGame->LoadText(300, 300, 500, 400, L"·µ»Ø"));
	::SWinGame->Loadbitmap(300, 100, 500, 300, L"C:\\Users\\H\\Desktop\\WindowsProject1\\x64\\Debug\\Resource\\Win.png");
}

void Load_D2DResource()
{
	Load_SWinGame();
}