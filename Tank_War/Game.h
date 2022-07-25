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

	int key1[5];		//p1��λ�洢
	int key2[5];		//p2��λ�洢
public:
	Game() {}
	void Init_Game(int id = ::Cur_Map_id, int my_id = my_tank_location);
	void Move();
	void Draw();
	void Get_keymap();		//��ȡ/���»�ȡ ��λ
	void Tank_Input();		//��Ӧ�������
	void Tank_Move(Tank* ptank);	//�ƶ�̹�ˣ�ͬʱ����ƶ��Ϸ��ԣ���ײ��飩
	void Bullet_Move(bullet* pbullet);		//�ӵ����ƶ�


	void online();
	void send_bullet();
	void send_mytankinfo();
	void send_hittank(int id, bullet* pbullet);
	void send_hitbrick(int id, bullet* pbullet);
	void refrash_tankinfo(char ch[]);
	void refrash_bullet(char ch[]);

	void recv_hitbrick(char buf[]);
	void recv_hited(char buf[]);
	void recv_myhited();
	void recv_destoryed(char buf[]);
	void recv_mydestoryed();
	void is_end();
};

void set_My_id(int id);