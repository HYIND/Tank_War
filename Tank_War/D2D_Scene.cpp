#include "D2D_Scene.h"
#include "Network.h"

namespace Location
{
	RECT rect;
	int broder1;
	int broder2;
	int broder3;
	int broder4;
	int len_x;
	int len_y;
}

using namespace Location;

ID2D1SolidColorBrush* pWhite_Brush;
ID2D1SolidColorBrush* pBlack_Brush;

ID2D1SolidColorBrush* pbullet_Brush;

ID2D1SolidColorBrush* pHall_Brush;
ID2D1SolidColorBrush* pHall_ClickBrush;

ID2D1SolidColorBrush* pMain_Brush;
ID2D1SolidColorBrush* pMain_ClickBrush;

IDWriteTextFormat* pMain_Format;
IDWriteTextFormat* pHall_Format;
IDWriteTextFormat* pPing_Format;


ID2D1Bitmap* OP_pBitmap;
ID2D1Bitmap* TEXT_pBitmap;

D2D1_RECT_F DelayRect;	//	延迟显示位

Scene* CurScene;

Scene* SMain;
Scene* SHall;
Scene_Option* SOption;
Scene* SRoom_host;
Scene* SRoom_nothost;
Scene* SGaming_local;
Scene* SGaming_online;
Scene* SEndGame;
Scene* SWinGame;
Scene* SFailGame;
Scene* SPause;


HWND userid_in;
HWND Hall;
HWND Hall_room_list;
HWND Hall_user_list;
HWND Hall_edit_in;
HWND edit_hall;


HWND Room_user_list;
HWND Room_edit_in;
HWND edit_room;

STATUS status = STATUS::Main;

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
		pRenderTarget->DrawBitmap(v->pBitmap,
			RectF(v->Bitmap_location1, v->Bitmap_location2, v->Bitmap_location3, v->Bitmap_location4),
			v->opacity);
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
	else if (Bitmap_changed) {
		D2D_Button* Button = Bitmap_changed->pButton;
		if (MoveX < Button->Button_location1 || MoveX > Button->Button_location3 ||
			MoveY < Button->Button_location2 || MoveY > Button->Button_location4)
		{
			Bitmap_changed->Bitmap_location1 += 10;
			Bitmap_changed->Bitmap_location2 += 10;
			Bitmap_changed->Bitmap_location3 -= 10;
			Bitmap_changed->Bitmap_location4 -= 10;
			Bitmap_changed = NULL;
		}
		return;
	}
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
				else if (v->Bitmap) {
					v->Bitmap->Bitmap_location1 -= 10;
					v->Bitmap->Bitmap_location2 -= 10;
					v->Bitmap->Bitmap_location3 += 10;
					v->Bitmap->Bitmap_location4 += 10;
					Bitmap_changed = v->Bitmap;
				}
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
	else if (Bitmap_changed) {
		D2D_Button* Button = Bitmap_changed->pButton;
		if (ClickX > Button->Button_location1 && ClickX < Button->Button_location3 &&
			ClickY > Button->Button_location2 && ClickY < Button->Button_location4)
		{
			SendMessage(_hwnd, WM_COMMAND, Button->id, (LPARAM)_hwnd);
		}
	}
	else
	{
		for (auto& Button : Button_list)
		{
			if (ClickX > Button->Button_location1 && ClickX < Button->Button_location3 &&
				ClickY > Button->Button_location2 && ClickY < Button->Button_location4)
			{
				SendMessage(_hwnd, WM_COMMAND, Button->id, (LPARAM)_hwnd);
			}
		}
	}
}

// 修改按钮
bool Scene::ModifyButton_Location(int id, int loc1, int loc2, int loc3, int loc4, bool offset)
{
	for (auto& v : Button_list)
	{
		if (v->id == id)
		{
			if (offset)
			{
				v->Button_location1 += loc1;
				v->Button_location2 += loc2;
				v->Button_location3 += loc3;
				v->Button_location4 += loc4;
			}
			else {
				v->Button_location1 = loc1;
				v->Button_location2 = loc2;
				v->Button_location3 = loc3;
				v->Button_location4 = loc4;
			}
			return true;
		}
	}
	return false;
}
bool Scene::ModifyButton_ID(int oldid, int newid)
{
	for (auto& v : Button_list)
	{
		if (v->id == oldid)
		{
			v->id = newid;
			return true;
		}
	}
	return false;
}
//修改文字
bool Scene::ModifyText_byButton(int id, wstring newstr)
{
	for (auto& v : Button_list)
	{
		if (v->id == id)
		{
			if (v->Text)
			{
				v->Text->str = newstr;
				return true;
			}
			else return false;
		}
	}
	return false;
}
//修改位图信息
bool Scene::ModifyBitmap_byButton(int id, int loc1, int loc2, int loc3, int loc4, bool offset, float opcaity)
{
	for (auto& v : Button_list)
	{
		if (v->id == id)
		{
			if (v->Bitmap)
			{
				if (offset)
				{
					v->Bitmap->Bitmap_location1 += loc1;
					v->Bitmap->Bitmap_location2 += loc2;
					v->Bitmap->Bitmap_location3 += loc3;
					v->Bitmap->Bitmap_location4 += loc4;
				}
				else {
					v->Bitmap->Bitmap_location1 = loc1;
					v->Bitmap->Bitmap_location2 = loc2;
					v->Bitmap->Bitmap_location3 = loc3;
					v->Bitmap->Bitmap_location4 = loc4;
				}
				return true;
			}
			else return false;
		}
	}
	return false;
}

//删除按钮
bool Scene::DeleteButton(int id)
{
	for (auto button_it = Button_list.begin(); button_it != Button_list.end(); button_it++)
	{
		if ((*button_it)->id == id)
		{
			if ((*button_it)->Bitmap)
			{
				D2D_Bitmap* d2d_bitmap = (*button_it)->Bitmap;
				for (auto bitmap_it = Bitmap_list.begin(); bitmap_it != Bitmap_list.end(); bitmap_it++)
				{
					if ((*bitmap_it) == d2d_bitmap)
						Bitmap_list.erase(bitmap_it);
				}
			}
			if ((*button_it)->Text)
			{
				D2D_Text* d2d_text = (*button_it)->Text;
				for (auto text_it = Text_list.begin(); text_it != Text_list.end(); text_it++)
				{
					if ((*text_it) == d2d_text)
						Text_list.erase(text_it);
				}
			}

			D2D_Button* d2d_button = *button_it;
			Button_list.erase(button_it);
			delete(d2d_button);
			return true;
		}
	}
	return false;
}

void Scene_Option::DrawScene()
{
	Scene::DrawScene();

	//FPS选项
	{
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 3, broder2 + len_y * 3.5), 10, 10), pWhite_Brush);
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 5, broder2 + len_y * 3.5), 10, 10), pWhite_Brush);
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 7, broder2 + len_y * 3.5), 10, 10), pWhite_Brush);

		if (Fps == 30.0)
		{
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 3, broder2 + len_y * 3.5), 8, 8), pBlack_Brush);
		}
		else if (Fps == 60.0)
		{
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 5, broder2 + len_y * 3.5), 8, 8), pBlack_Brush);
		}
		else if (Fps == 144.0)
		{
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(broder1 + len_x * 7, broder2 + len_y * 3.5), 8, 8), pBlack_Brush);
		}
	}
}

void InitScene(ID2D1Factory*& pD2DFactory, ID2D1HwndRenderTarget*& pRenderTarget, IWICImagingFactory*& pIWICFactory, IDWriteFactory*& pDWriteFactory)
{
	try {
		::SMain = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SHall = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SOption = new Scene_Option(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SRoom_host = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SRoom_nothost = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SGaming_local = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SGaming_online = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SWinGame = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SFailGame = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
		::SPause = new Scene(pD2DFactory, pRenderTarget, pIWICFactory, pDWriteFactory);
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
		IDB_LOCALGAME,
		SMain->LoadText(loc1, 80, loc3, 130, L"开始游戏"));

	SMain->LoadButton(loc1, 180, loc3, 230,
		IDB_ENTERHALL,
		SMain->LoadText(loc1, 180, loc3, 230, L"联机大厅"));

	SMain->LoadButton(loc1, 280, loc3, 330,
		IDB_OPTION,
		SMain->LoadText(loc1, 280, loc3, 330, L"设置"));

	SMain->LoadButton(loc1, 380, loc3, 430,
		IDB_QUITGAME,
		SMain->LoadText(loc1, 380, loc3, 430, L"退出游戏"));
}
void Load_SHall(RECT& rect)
{
	SHall->LoadResourceBitmap(broder1, broder2, broder3, broder4, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.6f);

	{
		SHall->LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
			IDB_EXITHALL,
			SHall->LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
	}

	{
		SHall->LoadText(broder1 + len_x, broder2,
			broder1 + len_x * 4, broder2 + len_y - 3,
			L"房间列表", pHall_Brush, pHall_Brush, pHall_Format);
		SHall->LoadText(broder1 + len_x * 7 - 5, broder2 + 5,
			broder1 + len_x * 9 + 5, broder2 + len_y + 3,
			L"大厅用户列表", pHall_Brush, pHall_Brush, pHall_Format);
	}

	{
		SHall->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
			IDB_ENTERROOM,
			SHall->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
				L"加入房间", pHall_Brush, pHall_ClickBrush, pHall_Format));

		SHall->LoadButton(broder1 + len_x * 8, broder2 + len_y * 5 + 10, broder3 - 10, broder2 + len_y * 6,
			IDB_CREATEROOM,
			SHall->LoadText(broder1 + len_x * 8, broder2 + len_y * 5 + 10, broder3 - 10, broder2 + len_y * 6,
				L"创建房间", pHall_Brush, pHall_ClickBrush, pHall_Format));

		SHall->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 6 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 7,
			IDB_REFRESH,
			SHall->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 6 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 7,
				L"刷新", pHall_Brush, pHall_ClickBrush, pHall_Format));

		SHall->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
			IDB_HALL_SEND,
			SHall->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
				L"发送", pHall_Brush, pHall_ClickBrush, pHall_Format));
	}

	{
		Hall_room_list = CreateWindowW(L"LISTBOX", L"",
			WS_CHILD,
			broder1 + len_x, broder2 + len_y - 5,
			len_x * 4, len_y * 4,
			_hwnd, (HMENU)HALL_ROOM_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		Hall_user_list = CreateWindowW(L"LISTBOX", L"",
			WS_CHILD,
			broder1 + len_x * 7, broder2 + len_y + 5,
			len_x * 2, len_y * 3,
			_hwnd, (HMENU)HALL_USER_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		edit_hall = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_LEFT | WS_VSCROLL | ES_READONLY,
			broder1 + len_x, broder2 + len_y * 5,
			len_x * 5, len_y * 3 - 10,
			_hwnd, (HMENU)EDIT_HALL, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		Hall_edit_in = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_LEFT | WS_VSCROLL,
			broder1 + len_x, broder2 + len_y * 8,
			len_x * 5, len_y * 2 - 10,
			_hwnd, (HMENU)HALL_EDIT_IN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
	}

}
void Load_Soption(RECT& rect)
{
	SOption->LoadResourceBitmap(broder1, broder2, broder3, broder4, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.6f);

	{
		SOption->LoadText(broder1 + len_x * 3, broder2 + len_y,
			broder1 + len_x * 5, broder2 + len_y * 2,
			L"设置", pHall_Brush, pHall_Brush, pHall_Format);

		SOption->LoadText(broder1 + len_x * 1, broder2 + len_y * 3,
			broder1 + len_x * 2, broder2 + len_y * 4,
			L"帧率", pHall_Brush, pHall_Brush, pHall_Format);

		SOption->LoadText(broder1 + len_x * 3 + 10, broder2 + len_y * 3,
			broder1 + len_x * 4, broder2 + len_y * 4,
			L"30", pWhite_Brush, pWhite_Brush, pPing_Format);
		SOption->LoadText(broder1 + len_x * 5 + 10, broder2 + len_y * 3,
			broder1 + len_x * 6, broder2 + len_y * 4,
			L"60", pWhite_Brush, pWhite_Brush, pPing_Format);
		SOption->LoadText(broder1 + len_x * 7 + 10, broder2 + len_y * 3,
			broder1 + len_x * 8, broder2 + len_y * 4,
			L"144", pWhite_Brush, pWhite_Brush, pPing_Format);
	}

	{
		SOption->LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
			IDB_EXITOPTION,
			SOption->LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
	}

	{
		SOption->LoadButton(broder1 + len_x * 3 - 10, broder2 + len_y * 3.5 - 10,
			broder1 + len_x * 3 + 10, broder2 + len_y * 3.5 + 10,
			IDB_SETFPS_30);
		SOption->LoadButton(broder1 + len_x * 5 - 10, broder2 + len_y * 3.5 - 10,
			broder1 + len_x * 5 + 10, broder2 + len_y * 3.5 + 10,
			IDB_SETFPS_60);
		SOption->LoadButton(broder1 + len_x * 7 - 10, broder2 + len_y * 3.5 - 10,
			broder1 + len_x * 7 + 10, broder2 + len_y * 3.5 + 10,
			IDB_SETFPS_144);
	}
}
void Load_SRoom(RECT& rect)
{

	{
		SRoom_host->LoadResourceBitmap(broder1, broder2, broder3, broder4, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.6f);

		{
			SRoom_host->LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
				IDB_EXITROOM,
				SRoom_host->LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
		}

		{
			SRoom_host->LoadText(broder1 + len_x, broder2,
				broder1 + len_x * 4, broder2 + len_y - 3,
				L"当前房间内玩家情况", pHall_Brush, pHall_Brush, pHall_Format);
		}

		{
			SRoom_host->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
				IDB_STARTGAME,
				SRoom_host->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
					L"开始游戏", pHall_Brush, pHall_ClickBrush, pHall_Format));

			SRoom_host->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
				IDB_ROOM_SEND,
				SRoom_host->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
					L"发送", pHall_Brush, pHall_ClickBrush, pHall_Format));
		}
	}


	{
		SRoom_nothost->LoadResourceBitmap(broder1, broder2, broder3, broder4, L"PNG", MAKEINTRESOURCE(TEXTBK_PNG), 0.6f);

		{
			SRoom_nothost->LoadText(broder1 + len_x, broder2,
				broder1 + len_x * 4, broder2 + len_y - 3,
				L"当前房间内玩家情况", pHall_Brush, pHall_Brush, pHall_Format);
		}
		{
			SRoom_nothost->LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
				IDB_EXITROOM,
				SRoom_nothost->LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
		}

		{
			SRoom_nothost->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
				IDB_READY,
				SRoom_nothost->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 5 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 6,
					L"准备", pHall_Brush, pHall_ClickBrush, pHall_Format));

			SRoom_nothost->LoadButton(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
				IDB_ROOM_SEND,
				SRoom_nothost->LoadText(broder1 + len_x * 6 + 10, broder2 + len_y * 8 + 10, broder1 + len_x * 8 - 10, broder2 + len_y * 9,
					L"发送", pHall_Brush, pHall_ClickBrush, pHall_Format));
		}
	}


	{

		Room_user_list = CreateWindowW(L"LISTBOX", L"",
			WS_CHILD,
			broder1 + len_x, broder2 + len_y - 5,
			len_x * 4, len_y * 4,
			_hwnd, (HMENU)ROOM_USER_LIST, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);

		edit_room = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_LEFT | WS_VSCROLL | ES_READONLY,
			broder1 + len_x, broder2 + len_y * 5,
			len_x * 5, len_y * 3 - 10,
			_hwnd, (HMENU)EDIT_HALL, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
		Room_edit_in = CreateWindowW(L"EDIT", L"",
			WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_LEFT | WS_VSCROLL,
			broder1 + len_x, broder2 + len_y * 8,
			len_x * 5, len_y * 2 - 10,
			_hwnd, (HMENU)ROOM_EDIT_IN, (HINSTANCE)GetWindowLong(_hwnd, GWLP_HINSTANCE), NULL);
	}
}
void Load_SGaming(RECT& rect)
{
	{
		SGaming_local->LoadButton(rect.left, rect.top, rect.left + 100, rect.top + 67,
			IDB_PAUSE,
			SGaming_local->LoadResourceBitmap(rect.left, rect.top, rect.left + 100, rect.top + 67, L"PNG", MAKEINTRESOURCE(PAUSE_PNG)));
	}


	{
		SGaming_online->LoadButton(rect.left, rect.top, rect.left + 144, rect.top + 87,
			IDB_RETURN,
			SGaming_online->LoadResourceBitmap(rect.left, rect.top, rect.left + 144, rect.top + 87, L"PNG", MAKEINTRESOURCE(RETURN_PNG)));
	}
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
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"JPG", MAKEINTRESOURCE(OPBK_JPG), &OP_pBitmap);
	Load_SMain(rect);
	Load_SHall(rect);
	Load_Soption(rect);
	Load_SRoom(rect);
	//Load_EndGame(rect);
	Load_SWinGame(rect);
	Load_SFailGame(rect);
	Load_SGaming(rect);
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
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &pWhite_Brush);
	hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &pBlack_Brush);

	hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 0, 0, 1), &pbullet_Brush);

	pMain_Brush = pBlack_Brush;
	pMain_ClickBrush = pWhite_Brush;
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &pMain_Brush);
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &pMain_ClickBrush);

	pHall_Brush = pBlack_Brush;
	pHall_ClickBrush = pWhite_Brush;
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(1, 1, 1, 1), &pHall_Brush);
	//hr = pRenderTarget->CreateSolidColorBrush(ColorF(0, 0, 0, 1), &pHall_ClickBrush);


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

	hr = pPing_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hr = pPing_Format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	DelayRect = RectF(rect.right - 60, rect.top + 5, rect.right - 5, rect.top + 30);
}

void Init_D2DResource()
{
	GetClientRect(_hwnd, &rect);

	broder1 = rect.left + 70;
	broder2 = rect.top + 30;
	broder3 = rect.right - 70;
	broder4 = rect.bottom - 30;
	len_x = (broder3 - broder1) / 10;
	len_y = (broder4 - broder2) / 10;

	Init_D2DTool(rect);
	InitScene(pD2DFactory, pRenderTarget, pIWICFactory, pIDWriteFactory);
	Load_D2DUI(rect);
}

void Show_Hall(bool flag)
{
	if (flag)
	{
		(int)SendMessage(Hall_room_list, LB_RESETCONTENT, 0, 0);
		(int)SendMessage(Hall_user_list, LB_RESETCONTENT, 0, 0);
		(int)SendMessage(edit_hall, WM_SETTEXT, 0, (LPARAM)L"");
		(int)SendMessage(Hall_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
		ShowWindow(edit_hall, SW_SHOW);
		ShowWindow(Hall_edit_in, SW_SHOW);
		ShowWindow(Hall_room_list, SW_SHOW);
		ShowWindow(Hall_user_list, SW_SHOW);
	}
	else
	{
		ShowWindow(edit_hall, SW_HIDE);
		ShowWindow(Hall_edit_in, SW_HIDE);
		ShowWindow(Hall_room_list, SW_HIDE);
		ShowWindow(Hall_user_list, SW_HIDE);
	}
}
void Show_Room(bool flag)
{
	if (flag)
	{
		(int)SendMessage(Room_user_list, LB_RESETCONTENT, 0, 0);
		(int)SendMessage(edit_room, WM_SETTEXT, 0, (LPARAM)L"");
		(int)SendMessage(Room_edit_in, WM_SETTEXT, 0, (LPARAM)L"");
		ShowWindow(edit_room, SW_SHOW);
		ShowWindow(Room_edit_in, SW_SHOW);
		ShowWindow(Room_user_list, SW_SHOW);
	}
	else
	{
		ShowWindow(edit_room, SW_HIDE);
		ShowWindow(Room_edit_in, SW_HIDE);
		ShowWindow(Room_user_list, SW_HIDE);
	}
}


void Set_CurScene(STATUS status_in)
{
	Show_Hall(false);
	Show_Room(false);
	switch (status_in)
	{
	case STATUS::Room_Status:
	{
		isonline_game = false;
		isstart = false;
		status = STATUS::Room_Status;
		if (!host)
		{
			if (isready)
			{
				isready = false;
				SRoom_nothost->ModifyButton_ID(IDB_CANCELREADY, IDB_READY);
				SRoom_nothost->ModifyText_byButton(IDB_READY, L"准备");
			}
			CurScene = SRoom_nothost;
		}
		else
		{
			CurScene = SRoom_host;
		}
		Show_Room(TRUE);
		Get_Room_Info();
		break;
	}
	case STATUS::Main:
	{
		isonline_game = false;
		isready = false;
		isstart = false;
		host = false;
		status = STATUS::Main;
		CurScene = SMain;
		break;
	}
	case STATUS::Option:
	{
		status = STATUS::Option;
		CurScene = SOption;
		break;
	}
	case STATUS::Hall_Status:
	{
		isonline_game = false;
		isready = false;
		isstart = false;
		host = false;
		status = STATUS::Hall_Status;
		CurScene = SHall;
		Show_Hall(true);
		Get_Hallinfo();
		break;
	}
	case STATUS::Game_Status:
	{
		isstart = true;
		status = STATUS::Game_Status;
		if (isonline_game)
			CurScene = SGaming_online;
		else CurScene = SGaming_local;
	}
	default:
		break;
	}
}