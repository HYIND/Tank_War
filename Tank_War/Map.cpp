#include "Map.h"

int Cur_Map_id = 0;
map<int, Map> Map_list;

#define SHOW(Vec) for (auto& v : Vec){v.Draw();}

ID2D1Bitmap* Zero_BK;

/* 以下为外部声明 */
extern HINSTANCE hInst;

void Init_Map_Zero()
{
	int count = 1;
	Map Default_Map(0, 2);
	Default_Map.BK_pBitmap = Zero_BK;
	Default_Map.Init_Location.emplace_back(1, 50, 300, UP, TankStyle::DEFAULT);
	Default_Map.Init_Location.emplace_back(2, 500, 300, UP, TankStyle::DEFAULT);
	for (int i = 0; i < 10; i++)
	{
		Brick_Wall brick(100 + i * 50, 200, count);
		Default_Map.Brick_info.emplace_back(brick);
		count++;
	}
	for (int i = 0; i < 10; i++)
	{
		Iron_Wall iron(400, 200 + i * 50, count);
		Default_Map.Iron_info.emplace_back(iron);
		count++;
	}
	Map_list[Default_Map.map_id] = Default_Map;
}

void Init_Map()
{
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"JPG", MAKEINTRESOURCE(BK_SAND), &Zero_BK);
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BRICK_WALL), &brick_wall_pBitmap);
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(IRON_WALL), &iron_wall_pBitmap);
	Init_Map_Zero();
}

void Map::DrawMap()
{
	//SHOW(Brick_info);
	//SHOW(Iron_info);
}