#include "Scene.h"
#include "Map.h"
#include "FileIO.h"


map <m_RECT*, component_type> TypeMap = {
	{new m_RECT(800,850,20,70),component_type::BRICK},
	{new m_RECT(900,950,20,70),component_type::IRON}
};
map <m_RECT*, TankStyle> TankMap = {
	{new m_RECT(1000,1050,20,70),TankStyle::DEFAULT},
};

void Scene_MapEdit::OnTick() {
	if (GetAsyncKeyState(0x41) & 0x8000)
	{
		this->curRotate -= 3.0;
	}
	else if (GetAsyncKeyState(0x44) & 0x8000)
	{
		this->curRotate += 3.0;
	}
}

void Scene_MapEdit::Active()
{
	this->curCom = component_type::DEFAULT;
	this->curStyle = TankStyle::NULLSTYLE;
	this->curRotate = 0.0;
	if (this->m_Map)
		this->m_Map->Clear();
	else this->m_Map = new Map();
}

void Scene_MapEdit::UnActiveSelect()
{
	this->curCom = component_type::DEFAULT;
	this->curStyle = TankStyle::NULLSTYLE;
	this->curRotate = 0.0;
}

void Scene_MapEdit::DrawMapEdit(double time_diff) {
	pRenderTarget->DrawBitmap(ResFactory->GetBitMapRes(ResName::opBK), D2D1::RectF(0, 0, _rect.right, _rect.bottom));
	if (m_Map)
		m_Map->DrawMap();
	for (auto& it : TypeMap)
	{
		m_RECT rect = *(it.first);
		if (com_info.find(it.second) != com_info.end())
			pRenderTarget->DrawBitmap(com_info[it.second]->Bitmap, D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom));
	}
	for (auto& it : TankMap)
	{
		m_RECT rect = *(it.first);
		if (Tank_Style_info.find(it.second) != Tank_Style_info.end())
		{
			D2D1_POINT_2F center = D2D1::Point2F(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(0 + 180, center));
			pRenderTarget->DrawBitmap(Tank_Style_info[it.second]->Bitmap, D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom));
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		}
		break;
	}

	for (auto& v : this->m_Map->Init_Location)
	{
		if (Tank_Style_info.find(v.tank_style) != Tank_Style_info.end())
		{
			D2D1_POINT_2F center = D2D1::Point2F(v.x, v.y);
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(v.rotate + 180, center));
			pRenderTarget->DrawBitmap(Tank_Style_info[v.tank_style]->Bitmap, D2D1::RectF(
				v.x - Tank_Style_info[v.tank_style]->width / 2,
				v.y - Tank_Style_info[v.tank_style]->height / 2,
				v.x + Tank_Style_info[v.tank_style]->width / 2,
				v.y + Tank_Style_info[v.tank_style]->height / 2)
			);
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		}
	}

	if (curCom != component_type::DEFAULT)
	{
		if (com_info.find(curCom) != com_info.end())
			pRenderTarget->DrawBitmap(com_info[curCom]->Bitmap, D2D1::RectF(
				MousePos::MoveX - IRON_WIDTH / 2,
				MousePos::MoveY - IRON_HEIGHT / 2,
				MousePos::MoveX + IRON_WIDTH / 2,
				MousePos::MoveY + IRON_HEIGHT / 2)
			);
	}
	if (curStyle != TankStyle::NULLSTYLE)
	{
		if (Tank_Style_info.find(curStyle) != Tank_Style_info.end())
		{
			D2D1_POINT_2F center = D2D1::Point2F(MousePos::MoveX, MousePos::MoveY);
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(curRotate + 180, center));
			pRenderTarget->DrawBitmap(Tank_Style_info[curStyle]->Bitmap, D2D1::RectF(
				MousePos::MoveX - Tank_Style_info[curStyle]->width / 2,
				MousePos::MoveY - Tank_Style_info[curStyle]->height / 2,
				MousePos::MoveX + Tank_Style_info[curStyle]->width / 2,
				MousePos::MoveY + Tank_Style_info[curStyle]->height / 2)
			);
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		}
	}

}

void Scene_MapEdit::OnClick(bool isLButtonPress, bool isShiftPress) {
	Scene::OnClick(isLButtonPress, isShiftPress);
	//未选中物件
	if (curCom == component_type::DEFAULT && curStyle == TankStyle::NULLSTYLE)
	{
		component_type com = component_type::DEFAULT;
		TankStyle tank = TankStyle::NULLSTYLE;
		for (auto& it : TypeMap)
		{
			m_RECT rect = *(it.first);
			if (MousePos::ClickX > rect.left && MousePos::ClickX <rect.right &&
				MousePos::ClickY >rect.top && MousePos::ClickY < rect.bottom)
			{
				com = it.second;
				break;
			}
		}
		for (auto& it : TankMap)
		{
			m_RECT rect = *(it.first);
			if (MousePos::ClickX > rect.left && MousePos::ClickX <rect.right &&
				MousePos::ClickY >rect.top && MousePos::ClickY < rect.bottom)
			{
				tank = it.second;
				break;
			}
		}
		this->selectCom(com, tank);
	}
	else {//选中物件
		if (!isLButtonPress)
			UnActiveSelect();
		else {
			this->AddCom(MousePos::ClickX, MousePos::ClickY);
			if (!isShiftPress)
				UnActiveSelect();
		}
	}
}

void Scene_MapEdit::selectCom(component_type type, TankStyle style) {
	if (style != TankStyle::NULLSTYLE)
	{
		this->curStyle = style;
		this->curCom = component_type::DEFAULT;
	}
	else if (type != component_type::DEFAULT) {
		this->curCom = type;
		this->curStyle = TankStyle::NULLSTYLE;
	}
}

void Scene_MapEdit::AddCom(int X, int Y) {
	if (this->curStyle != TankStyle::NULLSTYLE)
	{
		int id = this->m_Map->Init_Location.size() + 1;
		this->m_Map->Init_Location.emplace_back(id, X, Y, this->curRotate, this->curStyle);
	}
	else if (this->curCom != component_type::DEFAULT)
	{
		Game_Component* com = nullptr;
		int id = this->m_Map->Init_Location.size() + 1;
		switch (this->curCom)
		{
		case component_type::BRICK:
		{
			Brick_Wall* brack = new Brick_Wall(X, Y, id);
			com = (Game_Component*)brack;
			break;
		}
		case component_type::IRON:
		{
			Iron_Wall* iron = new Iron_Wall(X, Y, id);
			com = (Game_Component*)iron;
			break;
		}
		default:
			break;
		}
		this->m_Map->Component_info.emplace_back(com);
	}
}

void Scene_MapEdit::ReadMapFile()
{
	TCHAR* filename = nullptr;
	char* buf = nullptr;
	int len = 0;
	if (FileIO::OpenOneFile(filename) && FileIO::LoadFile(filename, buf, len))
	{
		if (this->m_Map->Load(buf, len)) {
			//MessageBox(NULL, TEXT("加载成功！"), TEXT("OK"), MB_OK);
		}
		else {
			MessageBox(NULL, TEXT("加载失败，请检查文件是否正确！"), TEXT("Error"), MB_OK);
		}
	}
	if (filename)
		delete filename;
	if (buf)
		delete buf;
}

void Scene_MapEdit::SaveMapFile()
{
	TCHAR* filename = nullptr;
	if (FileIO::SaveOneFile(filename)) {
		char* buf = nullptr;
		int len = 0;
		this->m_Map->Save(buf, len);
		if (len > 0 && buf)
			FileIO::SaveFile(filename, buf, len);
		if (buf)
			delete buf;
	}
	if (filename)
		delete filename;
}