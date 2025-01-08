#pragma once
#include "Map.h"
#include "Network.h"
#include "Tank_AI.h"
#include "Prop.h"
#include "GameScene_CollisionManager.h"

class AI_control;

extern int my_tank_location;

class Game
{
public:
	Map map_info;

	map<int, Tank*> Tank_info;
	vector<Prop* >Prop_info;
	vector<D2D_GIF*> Gif_list;

	int my_tankid = 0;
	int player_alive = 0;

	Tank* ptank1 = nullptr;
	Tank* ptank2 = nullptr;

	int key1[5];		//p1键位存储
	int key2[5];		//p2键位存储
public:
	static Game* Instance();
	void LoadLocalMap();
	void LoadMap(Map* map);
	void Init_Game(int id = ::Cur_Map_id, int my_id = my_tank_location);
	void Tick();
	void Draw(double time_diff);
	void DrawGIF(double time_diff);
	void DrawTank();
	void Get_keymap();		//获取/重新获取 键位
	void Tank_Input();		//响应玩家输入
	void Tank_shot(Tank* ptank);		//发射子弹
	void Tank_Move(Tank* ptank, bool forward);		//移动坦克，同时检查移动合法性（碰撞检查）
	void Tank_Rotate(Tank* ptank, bool forward);	//旋转坦克，同时检查移动合法性（碰撞检查）
	void AI_Rotate(AI_control* AI, bool forward);	//AI用旋转
	void Bullet_Tick_Move(bullet* pbullet);			//子弹自移动

	D2D_GIF* AddGIF(int x1, int y1, int x2, int y2, GIFINFO* gifInfo, int loopCount = 1);


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
private:
	Game() {}
	void is_end();
};

void set_My_id(int id);