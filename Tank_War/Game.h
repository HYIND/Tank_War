#pragma once
#include "Map.h"

class Game;

extern Game* Cur_Game;
extern int my_tank_location;

class Game
{
public:
	Map map_info;
	map<int, Tank*> Tank_info;

	int my_tankid = 0;
	int player_alive = 0;

	Tank* ptank1 = nullptr;
	Tank* ptank2 = nullptr;
public:
	Game() {}
	void Init_Game(int id = ::Cur_Map_id, int my_id = my_tank_location);
	void Move();
	void Draw();
	void Tank_Input();
	void Tank_Move(Tank* ptank);
	void Bullet_Move(bullet* pbullet);
	void online();
	void send_bullet();
	void send_mytankinfo();
	void send_destory(int id, bullet* pbullet);
	void refrash_tankinfo(char ch[]);
	void refrash_bullet(char ch[]);
	void destoryed(char buf[]);
	void mydestoryed();
	void is_end();
};

void set_My_id(int id);