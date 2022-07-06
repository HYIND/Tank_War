#include "Game.h"

Game* Cur_Game = new Game();

void Game::Init_Game(int id)
{
	this->map_info = Map_list[id];
	for (auto& v : map_info.Init_Location)
	{
		Tank* tank = new Tank(v.x, v.y,
			Tank_Style_info[v.tank_style]->width,
			Tank_Style_info[v.tank_style]->height,
			v.direction, v.isalive);
		Tank_info[v.Tank_id] = tank;
	}
}

void Game::Move()
{
	if (isonline_game)
	{
	}
	else
	{
		if (GetAsyncKeyState('W') & 0x8000)
		{
			Tank_info[1]->Tank_Move(UP);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			Tank_info[1]->Tank_Move(LEFT);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			Tank_info[1]->Tank_Move(DOWN);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			Tank_info[1]->Tank_Move(RIGHT);
		}

		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			Tank_info[2]->Tank_Move(UP);
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			Tank_info[2]->Tank_Move(LEFT);
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			Tank_info[2]->Tank_Move(DOWN);
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			Tank_info[2]->Tank_Move(RIGHT);
		}
	}
}

void Game::Draw(bool isonline)
{
	if (isonline)
	{

	}
	else
	{
		for (auto& v : Tank_info)
		{
			if (v.second->isalive)
			{
				v.second->DrawTank();
			}
			if (v.second->bullet_head)
			{
				v.second->bullet_head->Drawbullet();
			}
		}
		map_info.DrawMap();
	}
}