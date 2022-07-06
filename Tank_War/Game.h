#pragma once
#include "Map.h"

class Game
{
public:
	Map map_info;
	map<int, Tank*> Tank_info;

	int my_tankid = 0;

public:
	Game() {}
	void Init_Game(int id = ::Cur_Map_id);
	void Move();
	void Draw(bool isonline = isonline_game);
};

extern Game* Cur_Game;