#include "D2D_Scene.h"

ID2D1Factory* pD2DFactory;
ID2D1HwndRenderTarget* pRenderTarget;
IWICImagingFactory* pIWICFactory;
IDWriteFactory* pIDWriteFactory;

ID2D1SolidColorBrush* bullet_pBrush;

ID2D1SolidColorBrush* pHall_Brush;
ID2D1SolidColorBrush* pHall_ClickBrush;

ID2D1SolidColorBrush* pMain_Brush;
ID2D1SolidColorBrush* pMain_ClickBrush;

IDWriteTextFormat* pMain_Format;
IDWriteTextFormat* pHall_Format;
IDWriteTextFormat* pPing_Format;

ID2D1Bitmap* OP_pBitmap;
ID2D1Bitmap* TEXT_pBitmap;

ID2D1Bitmap* P1_CurTank_Form;
ID2D1Bitmap* P2_CurTank_Form;
ID2D1Bitmap* DefTank_pBitmap;


D2D1_RECT_F DelayRect;	//	延迟显示位

Scene* CurScene;

Scene* SMain;
Scene* SHall;
Scene* SOption;
Scene* SRoom;
Scene* SPause;
Scene* SGaming;
Scene* SWinGame;
Scene* SFailGame;

extern LPCTSTR OP_Resource;
extern LPCTSTR Tank_Resource;
extern LPCTSTR TEXT_Resource;

extern HWND room_list;
extern HWND user_list;
extern HWND edit_hall;
extern HWND edit_in;

int MoveX, MoveY, ClickX, ClickY;

D2D_Bitmap* Scene::Loadbitmap(int loc1, int loc2, int loc3, int loc4, LPCTSTR pszResource, float opacity)
{
	D2D_Bitmap* Bitmap = new D2D_Bitmap(loc1, loc2, loc3, loc4, NULL, opacity);
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

D2D_Bitmap* Scene::LoadResourceBitmap(int loc1, int loc2, int loc3, int loc4,
	LPCWSTR resourceType, LPCWSTR resourceName, float opacity, HINSTANCE hinstance
)
{
	D2D_Bitmap* Bitmap = new D2D_Bitmap(loc1, loc2, loc3, loc4, NULL, opacity);
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
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;

	void* pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource.
	imageResHandle = FindResource((HMODULE)hinstance, resourceName, resourceType);;
	HRESULT hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(hinstance, imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);

		hr = pImageFile ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(hinstance, imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;

	}
	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pIWICFactory->CreateStream(&pStream);
	}
	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
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
		if (SUCCEEDED(hr))
		{
			//create a Direct2D bitmap from the WIC bitmap.
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
		pRenderTarget->DrawBitmap(v->pBitmap, RectF(v->Bitmap_location1, v->Bitmap_location2, v->Bitmap_location3, v->Bitmap_location4), v->opacity);
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

HRESULT Loadbitmap(IWICImagingFactory* pIWICFactory, ID2D1RenderTarget* pRenderTarget,
	LPCTSTR pszResource, ID2D1Bitmap** ppBitmap)
{
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
			ppBitmap
		);
	}

	SafeRelease(pDecoder);
	SafeRelease(pSource);
	SafeRelease(pStream);
	SafeRelease(pConverter);
	SafeRelease(pScaler);

	return hr;
}

HRESULT LoadResourceBitmap(
	HINSTANCE hinstance,
	IWICImagingFactory* pIWICFactory, ID2D1RenderTarget* pRenderTarget,
	LPCWSTR resourceType, LPCWSTR resourceName, ID2D1Bitmap** ppBitmap
)
{
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
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;

	void* pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource.
	DWORD k = GetLastError();
	imageResHandle = FindResource((HMODULE)hinstance, resourceName, resourceType);
	k = GetLastError();
	HRESULT hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(hinstance, imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);

		hr = pImageFile ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(hinstance, imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;

	}
	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pIWICFactory->CreateStream(&pStream);
	}
	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
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
		if (SUCCEEDED(hr))
		{
			//create a Direct2D bitmap from the WIC bitmap.
			hr = pRenderTarget->CreateBitmapFromWicBitmap(
				pConverter,
				NULL,
				ppBitmap
			);

		}

		SafeRelease(pDecoder);
		SafeRelease(pSource);
		SafeRelease(pStream);
		SafeRelease(pConverter);
		SafeRelease(pScaler);

		return hr;
	}
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

void Load_SMain(RECT& rect)
{
	int loc1 = (rect.left + rect.right) / 2 - 120;
	int loc3 = (rect.left + rect.right) / 2 + 120;
	SMain->LoadButton(loc1, 80, loc3, 130,
		IDB_TWO,
		SMain->LoadText(loc1, 80, loc3, 130, L"开始游戏"));

	SMain->LoadButton(loc1, 180, loc3, 230,
		IDB_THREE,
		SMain->LoadText(loc1, 180, loc3, 230, L"联机大厅"));

	SMain->LoadButton(loc1, 280, loc3, 330,
		IDB_FOUR,
		SMain->LoadText(loc1, 280, loc3, 330, L"设置"));

	SMain->LoadButton(loc1, 380, loc3, 430,
		IDB_FIVE,
		SMain->LoadText(loc1, 380, loc3, 430, L"退出游戏"));
}

void Load_SHall(RECT& rect)
{
	int broder1 = rect.left + 70;
	int broder2 = rect.top + 30;
	int broder3 = rect.right - 70;
	int broder4 = rect.bottom - 30;
	int len_x = (broder3 - broder1) / 10;
	int len_y = (broder4 - broder2) / 10;
	SHall->LoadResourceBitmap(broder1, broder2, broder3, broder4, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.6f);

	SHall->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
		IDB_ENTERROOM,
		SHall->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
			L"加入房间", pHall_Brush, pHall_ClickBrush, pHall_Format));

	SHall->LoadButton(broder1 + len_x * 8, broder2 + len_y * 5 + 10, broder3 - 10, broder2 + len_y * 6 - 10,
		IDB_CREATEROOM,
		SHall->LoadText(broder1 + len_x * 8, broder2 + len_y * 5 + 10, broder3 - 10, broder2 + len_y * 6 - 10,
			L"创建房间", pHall_Brush, pHall_ClickBrush, pHall_Format));

	SHall->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 6 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 7,
		IDB_REFRESH,
		SHall->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 6 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 7,
			L"刷新", pHall_Brush, pHall_ClickBrush, pHall_Format));


	SHall->LoadButton(broder1 + len_x * 8, broder2 + len_y * 6 + 10, broder3 - 10, broder2 + len_y * 7,
		IDB_EXITHALL,
		SHall->LoadText(broder1 + len_x * 8, broder2 + len_y * 6 + 10, broder3 - 10, broder2 + len_y * 7,
			L"退出大厅", pHall_Brush, pHall_ClickBrush, pHall_Format));

	SHall->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
		IDB_SEND,
		SHall->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
			L"发送", pHall_Brush, pHall_ClickBrush, pHall_Format));

	room_list = CreateWindowW(L"LISTBOX", L"",
		WS_CHILD,
		broder1 + len_x, broder2 + len_y - 10,
		len_x * 4, len_y * 4,
		_hwnd, (HMENU)ROOM_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
	user_list = CreateWindowW(L"LISTBOX", L"",
		WS_CHILD,
		broder1 + len_x * 7, broder2 + len_y,
		len_x * 2, len_y * 3,
		_hwnd, (HMENU)USER_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
	edit_hall = CreateWindowW(L"EDIT", L"",
		WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_LEFT | WS_VSCROLL | ES_READONLY,
		broder1 + len_x, broder2 + len_y * 5,
		len_x * 5, len_y * 3 - 10,
		_hwnd, (HMENU)EDIT_HALL, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
	edit_in = CreateWindowW(L"EDIT", L"",
		WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_LEFT | WS_VSCROLL,
		broder1 + len_x, broder2 + len_y * 8,
		len_x * 5, len_y * 2 - 10,
		_hwnd, (HMENU)EDIT_IN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);

}

void Load_SWinGame(RECT& rect)
{
	int middle = rect.left + (rect.right - rect.left) / 2;
	try
	{
		::SWinGame->LoadButton(middle - 100, 250, middle + 100, 350,
			ReturnInEndGame,
			::SWinGame->LoadText(middle - 100, 250, middle + 100, 350, L"返回"));
		::SWinGame->LoadResourceBitmap(middle - 120, 30, middle + 120, 230, L"PNG", MAKEINTRESOURCE(WIN_PNG));

	}
	catch (std::exception& e)
	{
		return;
	}
}

void Load_SRoom(RECT& rect)
{

}

void Load_SFailGame(RECT& rect)
{
	int middle = rect.left + (rect.right - rect.left) / 2;
	try
	{
		::SFailGame->LoadButton(middle - 100, 250, middle + 100, 350,
			ReturnInEndGame,
			::SFailGame->LoadText(middle - 100, 250, middle + 100, 350, L"返回"));
		::SFailGame->LoadResourceBitmap(middle - 100, 100, middle + 100, 190, L"PNG", MAKEINTRESOURCE(FAIL_PNG));

	}
	catch (std::exception& e)
	{
		return;
	}
}

void Load_D2DUI(RECT& rect)
{
	Load_SMain(rect);
	Load_SHall(rect);
	Load_SRoom(rect);
	Load_SWinGame(rect);
	Load_SFailGame(rect);
}

void Init_D2DTool(RECT& rect)
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	HRESULT hr = S_OK;

	hr = pD2DFactory->CreateHwndRenderTarget(
		RenderTargetProperties(),
		HwndRenderTargetProperties(_hwnd, SizeU(rect.right - rect.left, rect.bottom - rect.top)),
		&pRenderTarget
	);

	hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 0, 0, 1), &bullet_pBrush);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &pMain_Brush);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &pMain_ClickBrush);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &pHall_Brush);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &pHall_ClickBrush);


	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pIDWriteFactory)
	);

	hr = pIDWriteFactory->CreateTextFormat(
		L"SimSun",                   // Font family name
		NULL,                          // Font collection(NULL sets it to the system font collection)
		DWRITE_FONT_WEIGHT_REGULAR,    // Weight
		DWRITE_FONT_STYLE_NORMAL,      // Style
		DWRITE_FONT_STRETCH_NORMAL,    // Stretch
		50.0f,                         // Size    
		L"zh-cn",                      // Local
		&pMain_Format                 // Pointer to recieve the created object
	);
	hr = pMain_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hr = pMain_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	hr = pIDWriteFactory->CreateTextFormat(
		L"Gabriola",                   // Font family name
		NULL,                          // Font collection(NULL sets it to the system font collection)
		DWRITE_FONT_WEIGHT_REGULAR,    // Weight
		DWRITE_FONT_STYLE_NORMAL,      // Style
		DWRITE_FONT_STRETCH_NORMAL,    // Stretch
		25.0f,                         // Size    
		L"zh-cn",                      // Local
		&pHall_Format                 // Pointer to recieve the created object
	);

	hr = pIDWriteFactory->CreateTextFormat(
		L"SimHei",                   // Font family name
		NULL,                          // Font collection(NULL sets it to the system font collection)
		DWRITE_FONT_WEIGHT_REGULAR,    // Weight
		DWRITE_FONT_STYLE_NORMAL,      // Style
		DWRITE_FONT_STRETCH_NORMAL,    // Stretch
		20.0f,                         // Size    
		L"zh-cn",                      // Local
		&pPing_Format                 // Pointer to recieve the created object
	);

	hr = pHall_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hr = pHall_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	DelayRect = RectF(rect.right - 60, rect.top + 5, rect.right - 5, rect.top + 30);
}

void Init_D2DResource()
{
	RECT rect;
	GetClientRect(_hwnd, &rect);
	Init_D2DTool(rect);
	InitScene(pD2DFactory, pRenderTarget, pIWICFactory, pIDWriteFactory);
	Load_D2DUI(rect);
}

void Init_GameResource()
{
	HRESULT hr = S_OK;
	hr = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"JPG", MAKEINTRESOURCE(OPBK_JPG), &OP_pBitmap);
	hr = LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(TANK_PNG), &DefTank_pBitmap);
	P1_CurTank_Form = DefTank_pBitmap;
	P2_CurTank_Form = DefTank_pBitmap;
}