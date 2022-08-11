#pragma once
#include "Map.h"
#include "Network.h"

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

	int key1[5];		//p1键位存储
	int key2[5];		//p2键位存储
public:
	Game() {}
	void Init_Game(int id = ::Cur_Map_id, int my_id = my_tank_location);
	void Move();
	void Draw();
	void Get_keymap();		//获取/重新获取 键位
	void Tank_Input();		//响应玩家输入
	void Tank_Move(Tank* ptank, bool forward);		//移动坦克，同时检查移动合法性（碰撞检查）
	void Tank_Rotate(Tank* ptank, bool forward);	//旋转坦克，同时检查移动合法性（碰撞检查）
	void Bullet_Move(bullet* pbullet);		//子弹自移动


	void online();
	void send_bullet();
	void send_mytankinfo();
	void send_hittank(int id, bullet* pbullet);
	void send_hitbrick(int id, bullet* pbullet);

	void refreash_tankinfo(Header& header, char* content);
	void refreash_bullet(Header& header, char* content);
	void recv_hitbrick(Header& header, char* content);
	void recv_hited(Header& header, char* content);
	void recv_myhited();
	void recv_destoryed(Header& header, char* content);
	void recv_mydestoryed();
	void is_end();
};

void set_My_id(int id);