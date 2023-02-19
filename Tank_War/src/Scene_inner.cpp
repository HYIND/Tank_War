#include "Scene.h"

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
	OnDrawScene();
}

void Scene::OnDrawScene()
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
	OnMove();
}

void Scene::OnMove()
{
	if (Text_changed)
	{
		D2D_Button* Button = Text_changed->pButton;
		if (MousePos::MoveX < Button->Button_location1 || MousePos::MoveX > Button->Button_location3 ||
			MousePos::MoveY < Button->Button_location2 || MousePos::MoveY > Button->Button_location4)
		{
			swap(Text_changed->pDefaultBrush, Text_changed->pClickBrush);
			Text_changed = NULL;
		}
		return;
	}
	else if (Bitmap_changed) {
		D2D_Button* Button = Bitmap_changed->pButton;
		if (MousePos::MoveX < Button->Button_location1 || MousePos::MoveX > Button->Button_location3 ||
			MousePos::MoveY < Button->Button_location2 || MousePos::MoveY > Button->Button_location4)
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
			if (MousePos::MoveX > v->Button_location1 && MousePos::MoveX < v->Button_location3 &&
				MousePos::MoveY > v->Button_location2 && MousePos::MoveY < v->Button_location4)
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
	OnClick();
}

void Scene::OnClick()
{
	if (Text_changed)
	{
		D2D_Button* Button = Text_changed->pButton;
		if (MousePos::ClickX > Button->Button_location1 && MousePos::ClickX < Button->Button_location3 &&
			MousePos::ClickY > Button->Button_location2 && MousePos::ClickY < Button->Button_location4)
		{
			SendMessage(_hwnd, WM_COMMAND, Button->id, (LPARAM)_hwnd);
		}
	}
	else if (Bitmap_changed) {
		D2D_Button* Button = Bitmap_changed->pButton;
		if (MousePos::ClickX > Button->Button_location1 && MousePos::ClickX < Button->Button_location3 &&
			MousePos::ClickY > Button->Button_location2 && MousePos::ClickY < Button->Button_location4)
		{
			SendMessage(_hwnd, WM_COMMAND, Button->id, (LPARAM)_hwnd);
		}
	}
	else
	{
		for (auto& Button : Button_list)
		{
			if (MousePos::ClickX > Button->Button_location1 && MousePos::ClickX < Button->Button_location3 &&
				MousePos::ClickY > Button->Button_location2 && MousePos::ClickY < Button->Button_location4)
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
				v->Bitmap->opacity = opcaity;
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
