#include "Map.h"

extern RECT _rect;

int Cur_Map_id = 0;
map<int, Map*> Map_list;
#define SHOW(Vec) for (auto& v : Vec){v->Draw();}

/* 以下为外部声明 */
extern HINSTANCE hInst;

Map::Map() :BK_pBitmap(ResFactory->GetBitMapRes(ResName::sandBK)) {}
Map::Map(int id, int user_limited) : BK_pBitmap(ResFactory->GetBitMapRes(ResName::sandBK)), map_id(id), user_limited(user_limited) {}
Map::~Map() {
	for (auto& com : this->Component_info)
	{
		if (com)
			delete com;
		com = nullptr;
	}
}

Map& Map::operator=(Map& other) {
	this->map_id = other.map_id;
	this->user_limited = other.user_limited;
	this->Init_Location = other.Init_Location;
	this->BK_pBitmap = other.BK_pBitmap;
	this->Component_info.clear();
	for (auto& v : other.Component_info)
	{
		Game_Component* com_temp = v;
		Game_Component* pCom;
		bool dynamic_result = false;
		switch (com_temp->type)
		{
		case component_type::BRICK:
		{
			Brick_Wall* pbrick_temp = nullptr;
			if (pbrick_temp = dynamic_cast<Brick_Wall*>(com_temp))
			{
				dynamic_result = true;
				Brick_Wall* p = new Brick_Wall(*pbrick_temp);
				pCom = p;
			}
			break;
		}
		case component_type::IRON:
		{
			Iron_Wall* piron_temp = nullptr;
			if (piron_temp = dynamic_cast<Iron_Wall*>(com_temp))
			{
				dynamic_result = true;
				Iron_Wall* p = new Iron_Wall(*piron_temp);
				pCom = p;
			}
			break;
		}
		case component_type::AIDKIT:
		{
			Aid_kit* pAidkit_temp = nullptr;
			if (pAidkit_temp = dynamic_cast<Aid_kit*>(com_temp))
			{
				dynamic_result = true;
				Aid_kit* p = new Aid_kit(*pAidkit_temp);
				pCom = p;
			}
			break;
		}
		default:
			break;
		}
		if (dynamic_result)
			this->Component_info.emplace_back(pCom);
	}
	return *this;
}

void Map::DrawMap()
{
	if (BK_pBitmap)
		pRenderTarget->DrawBitmap(BK_pBitmap, D2D1::RectF(0, 0, _rect.right, _rect.bottom));
	SHOW(Component_info);
}

void Map::DrawMap(function<void()> callback)
{
	if (BK_pBitmap)
		pRenderTarget->DrawBitmap(BK_pBitmap, D2D1::RectF(0, 0, _rect.right, _rect.bottom));
	callback();
	SHOW(Component_info);
}

void Map::Clear()
{
	for (auto& com : this->Component_info)
	{
		if (com)
			delete com;
		com = nullptr;
	}
	this->Init_Location.clear();
	this->Component_info.clear();
	this->user_limited = 0;
}
void Map::Save(char*& buf, int& len) {
	static char authkey[11] = "pammapmmap";
	int offset = 0;
	int tankCount = this->Init_Location.size();
	int comCount = this->Component_info.size();

	len = 10 + 2 * sizeof(int) + tankCount * 33 + comCount * 36;
	buf = new char[len];
	memcpy(buf + offset, authkey, 10);
	offset += 10;
	memcpy(buf + offset, &tankCount, sizeof(tankCount));
	offset += sizeof(tankCount);
	memcpy(buf + offset, &comCount, sizeof(comCount));
	offset += sizeof(comCount);
	for (auto& v : this->Init_Location)
	{
		memcpy(buf + offset, &(v.Tank_id), sizeof(v.Tank_id));
		offset += sizeof(v.Tank_id);
		memcpy(buf + offset, &(v.x), sizeof(v.x));
		offset += sizeof(v.x);
		memcpy(buf + offset, &(v.y), sizeof(v.y));
		offset += sizeof(v.y);
		memcpy(buf + offset, &(v.rotate), sizeof(v.rotate));
		offset += sizeof(v.rotate);
		memcpy(buf + offset, &(v.tank_style), sizeof(v.tank_style));
		offset += sizeof(v.tank_style);
		memcpy(buf + offset, &(v.isalive), sizeof(v.isalive));
		offset += sizeof(v.isalive);
	}
	for (auto& v : this->Component_info)
	{
		int id = v->id;
		component_type type = v->type;
		double x = v->get_locationX();
		double y = v->get_locationY();
		int width = v->get_width();
		int height = v->get_height();
		int health = v->get_health();
		memcpy(buf + offset, &id, sizeof(id));
		offset += sizeof(id);
		memcpy(buf + offset, &type, sizeof(type));
		offset += sizeof(type);
		memcpy(buf + offset, &x, sizeof(x));
		offset += sizeof(x);
		memcpy(buf + offset, &y, sizeof(y));
		offset += sizeof(y);
		memcpy(buf + offset, &width, sizeof(width));
		offset += sizeof(width);
		memcpy(buf + offset, &height, sizeof(height));
		offset += sizeof(height);
		memcpy(buf + offset, &health, sizeof(health));
		offset += sizeof(health);
	}
}
bool Map::Load(const char* buf, const int len) {
	if (len < 10 || buf == nullptr)return false;

	static char authkey[11] = "pammapmmap";
	char authbuf[11];
	memset(authbuf, '\0', 11);
	memcpy(authbuf, buf, 10);
	int offset = 10;
	if (strcmp(authkey, authbuf) != 0)
		return false;
	try
	{
		this->Clear();
		int tankCount;
		memcpy(&tankCount, buf + offset, sizeof(tankCount));
		offset += sizeof(tankCount);
		this->user_limited = tankCount;
		int comCount;
		memcpy(&comCount, buf + offset, sizeof(comCount));
		offset += sizeof(comCount);
		for (int i = 0; i < tankCount; i++)
		{
			int tankId;
			double x, y, rotate;
			TankStyle tankStyle;
			bool isalive;
			memcpy(&tankId, buf + offset, sizeof(tankId));
			offset += sizeof(tankId);
			memcpy(&x, buf + offset, sizeof(x));
			offset += sizeof(x);
			memcpy(&y, buf + offset, sizeof(y));
			offset += sizeof(y);
			memcpy(&rotate, buf + offset, sizeof(rotate));
			offset += sizeof(rotate);
			memcpy(&tankStyle, buf + offset, sizeof(tankStyle));
			offset += sizeof(tankStyle);
			memcpy(&isalive, buf + offset, sizeof(isalive));
			offset += sizeof(isalive);
			Init_info tankInfo(tankId, x, y, rotate, tankStyle, isalive);
			this->Init_Location.emplace_back(tankInfo);
		}
		for (int i = 0; i < comCount; i++)
		{
			int id;
			component_type type;
			double x, y;
			int width, height, health;
			memcpy(&id, buf + offset, sizeof(id));
			offset += sizeof(id);
			memcpy(&type, buf + offset, sizeof(type));
			offset += sizeof(type);
			memcpy(&x, buf + offset, sizeof(x));
			offset += sizeof(x);
			memcpy(&y, buf + offset, sizeof(y));
			offset += sizeof(y);
			memcpy(&width, buf + offset, sizeof(width));
			offset += sizeof(width);
			memcpy(&height, buf + offset, sizeof(height));
			offset += sizeof(height);
			memcpy(&health, buf + offset, sizeof(health));
			offset += sizeof(health);

			Game_Component* com = nullptr;
			switch (type)
			{

			case component_type::BRICK:
			{
				Brick_Wall* brick = new Brick_Wall(x, y, id, health);
				com = (Game_Component*)brick;
				break;
			}
			case component_type::IRON:
			{
				Iron_Wall* iron = new Iron_Wall(x, y, id);
				com = (Game_Component*)iron;
				break;
			}
			case component_type::DEFAULT:
			case component_type::PROP_DEFAULT:
			case component_type::AIDKIT:
			default:
				break;
			}
			if (com)
				this->Component_info.push_back(com);
		}
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}

void Init_Map_Zero()
{
	Map* Default_Map = new Map(0, 2);

	Default_Map->BK_pBitmap = ResFactory->GetBitMapRes(ResName::sandBK);

	Default_Map->Init_Location.emplace_back(1, 50, 280, 0, TankStyle::DEFAULT);
	Default_Map->Init_Location.emplace_back(2, _rect.right - 50, 280, 0, TankStyle::DEFAULT);

	int brick_count = 1;
	int icon_count = 1;

	for (int i = 0; i < 12; i++)
	{
		if (i == 5 || i == 6)
		{
			Brick_Wall* brick = new Brick_Wall(280 + i * IRON_WIDTH, 110, brick_count);
			Default_Map->Component_info.emplace_back((Game_Component*)brick);
			brick_count++;
			continue;
		}
		Iron_Wall* iron = new Iron_Wall(280 + i * IRON_WIDTH, 110, icon_count);
		Default_Map->Component_info.emplace_back((Game_Component*)iron);
		icon_count++;
	}
	for (int i = 0; i < 12; i++)
	{
		if (i == 5 || i == 6)
		{
			Brick_Wall* brick = new Brick_Wall(280 + i * IRON_WIDTH, 110 + 7 * IRON_HEIGHT, brick_count);
			Default_Map->Component_info.emplace_back((Game_Component*)brick);
			brick_count++;
			continue;
		}
		Iron_Wall* iron = new Iron_Wall(280 + i * IRON_WIDTH, 110 + 7 * IRON_HEIGHT, icon_count);
		Default_Map->Component_info.emplace_back((Game_Component*)iron);
		icon_count++;
	}
	for (int i = 1; i < 7; i++)
	{
		if (i == 3 || i == 4)
		{
			Brick_Wall* brick = new Brick_Wall(280, 110 + i * IRON_HEIGHT, brick_count);
			Default_Map->Component_info.emplace_back((Game_Component*)brick);
			brick_count++;
			continue;
		}
		Iron_Wall* iron = new Iron_Wall(280, 110 + i * IRON_HEIGHT, icon_count);
		Default_Map->Component_info.emplace_back((Game_Component*)iron);
		icon_count++;
	}
	for (int i = 1; i < 7; i++)
	{
		if (i == 3 || i == 4)
		{
			Brick_Wall* brick = new Brick_Wall(280 + 11 * IRON_WIDTH, 110 + i * IRON_HEIGHT, brick_count);
			Default_Map->Component_info.emplace_back((Game_Component*)brick);
			brick_count++;
			continue;
		}
		Iron_Wall* iron = new Iron_Wall(280 + 11 * IRON_WIDTH, 110 + i * IRON_HEIGHT, icon_count);
		Default_Map->Component_info.emplace_back((Game_Component*)iron);
		icon_count++;
	}
	for (int i = 5; i < 7; i++)
	{
		for (int j = 3; j < 5; j++)
		{
			Iron_Wall* iron = new Iron_Wall(280 + i * IRON_WIDTH, 110 + j * IRON_HEIGHT, icon_count);
			Default_Map->Component_info.emplace_back((Game_Component*)iron);
			icon_count++;
		}
	}
	Map_list[Default_Map->map_id] = Default_Map;
}

void Init_Map_Debug()
{
	Map* Debug_Map = new Map(-1, 2);

	Debug_Map->BK_pBitmap = ResFactory->GetBitMapRes(ResName::sandBK);

	Debug_Map->Init_Location.emplace_back(1, 50, 280, 0, TankStyle::DEFAULT);
	Debug_Map->Init_Location.emplace_back(2, _rect.right - 50, 280, 0, TankStyle::DEFAULT);

	Map_list[Debug_Map->map_id] = Debug_Map;
}

void Init_Map()
{
	Init_Map_Zero();
	Init_Map_Debug();
}
