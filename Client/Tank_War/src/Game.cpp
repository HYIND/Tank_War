#include "Game.h"
#include "keymap.h"
#include "FileIO.h"

#define _USE_MATH_DEFINES
#include <math.h>

int my_tank_location = 1;

Game* Game::Instance()
{
	static Game* m_Instance = new Game();
	return m_Instance;
}

void Game::LoadLocalMap() {
	TCHAR* filename = nullptr;
	char* buf = nullptr;
	int len = 0;
	if (FileIO::OpenOneFile(filename) && FileIO::LoadFile(filename, buf, len))
	{
		Map map;
		if (map.Load(buf, len)) {
			LoadMap(&map);
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

void Game::LoadMap(Map* map) {
	if (!map)
		return;

	Tank_info.clear();
	this->map_info = *map;
	GameSceneManager::Instance()->Build_Collision_Tree(this->map_info);
	for (auto& v : map_info.Init_Location)
	{
		Tank* tank = new Tank(v.x, v.y,
			Tank_Style_info[v.tank_style]->width,
			Tank_Style_info[v.tank_style]->height,
			v.rotate,
			Tank_Style_info[v.tank_style]->speed,
			v.isalive);
		Tank_info[v.Tank_id] = tank;
	}
	this->player_alive = map_info.user_limited;
	if (!isonline_game)
	{
		ptank1 = Tank_info[1];
		ptank2 = Tank_info[2];
	}
	else
	{
		ptank1 = Tank_info[my_tankid];
	}
}

void Game::Init_Game(int map_id, int my_id)
{
	this->my_tankid = my_id;
	Get_keymap();
	LoadMap(Map_list[map_id]);
}

void Game::Get_keymap()
{
	key1[0] = key_map_p1[keybroad::UP];
	key1[1] = key_map_p1[keybroad::DOWN];
	key1[2] = key_map_p1[keybroad::LEFT];
	key1[3] = key_map_p1[keybroad::RIGHT];
	key1[4] = key_map_p1[keybroad::FIRE];

	key2[0] = key_map_p2[keybroad::UP];
	key2[1] = key_map_p2[keybroad::DOWN];
	key2[2] = key_map_p2[keybroad::LEFT];
	key2[3] = key_map_p2[keybroad::RIGHT];
	key2[4] = key_map_p2[keybroad::FIRE];
}

void Game::Tick()
{
	Tank_Input();

	if (!isonline_game)
	{
		for (auto& v : Tank_info)
		{
			if (v.second->bullet_head)
				Bullet_Tick_Move(v.second->bullet_head);
		}
	}
	else
	{
		if (ptank1->bullet_head)
			Bullet_Tick_Move(ptank1->bullet_head);
	}
}

void Game::Draw(double time_diff)
{
	map_info.DrawMap(std::bind(&Game::DrawTank, this));
	DrawGIF(time_diff);
	return;
}

void Game::DrawGIF(double time_diff)
{
	for (auto it = Gif_list.begin(); it != Gif_list.end();)
	{
		if (!(*it)->Draw(time_diff))
			it = Gif_list.erase(it);
		else it++;
	}
}

void Game::DrawTank() {
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
}

void Game::online()
{
	send_mytankinfo();
	send_bullet();
}

void Game::send_hittank(int id, bullet* pbullet)
{
	Message::Game_hit_tank_Request Req;
	Req.set_hited_tank_id(id);

	Message::bulletinfo* binfo = new Message::bulletinfo();
	binfo->set_locationx(pbullet->get_locationX());
	binfo->set_locationy(pbullet->get_locationY());
	binfo->set_rotate(pbullet->get_rotate());
	binfo->set_bullet_style((int)pbullet->bullet_style);
	Req.set_allocated_bulletinfo(binfo);

	NetManager::Instance()->Send(Req);
}

void Game::send_hitbrick(int id, bullet* pbullet)
{
	Message::Game_hit_brick_Request Req;
	Req.set_hited_brick_id(id);

	Message::bulletinfo* binfo = new Message::bulletinfo();
	binfo->set_locationx(pbullet->get_locationX());
	binfo->set_locationy(pbullet->get_locationY());
	binfo->set_rotate(pbullet->get_rotate());
	binfo->set_bullet_style((int)pbullet->bullet_style);
	Req.set_allocated_bulletinfo(binfo);

	NetManager::Instance()->Send(Req);
}

void Game::send_mytankinfo()
{
	Message::Game_tankinfo_Request Req;
	Req.set_locationx(ptank1->get_locationX());
	Req.set_locationy(ptank1->get_locationY());
	Req.set_rotate(ptank1->get_rotate());
	Req.set_tank_style((int)ptank1->tank_style);

	NetManager::Instance()->Send(Req);
}

void Game::send_bullet()
{
	bullet* cur = ptank1->bullet_head;
	Message::Game_bulletinfo_Request Req;
	while (cur != NULL)
	{
		Message::bulletinfo* bulletinfo = Req.add_bulletinfo();
		bulletinfo->set_locationx(cur->get_locationX());
		bulletinfo->set_locationy(cur->get_locationY());
		bulletinfo->set_rotate(cur->get_rotate());
		bulletinfo->set_bullet_style((int)cur->bullet_style);
		cur = cur->next;
	}
	NetManager::Instance()->Send(Req);
}

void Game::refreash_tankinfo(Header& header, char* content)
{
	Message::Game_tankinfo_Response Res;
	Res.ParseFromArray(content, header.length);

	for (int i = 0; i < Res.info_size(); i++)
	{
		Message::Game_tankinfo_Response_tankinfo tankinfo = Res.info(i);
		Message::Game_tankinfo_Request* info = tankinfo.mutable_tankinfo();
		Tank* ptank = Tank_info[tankinfo.id()];
		if (ptank == ptank1) continue;
		ptank->set_locationX(info->locationx());
		ptank->set_locationY(info->locationy());
		ptank->set_rotate(info->rotate());
		ptank->tank_style = (TankStyle)info->tank_style();
		ptank->Set_Parameter_byStyle(ptank->tank_style);
	}
}

void Game::refreash_bullet(Header& header, char* content)
{
	Message::Game_bulletinfo_Response Res;
	Res.ParseFromArray(content, header.length);

	for (int i = 0; i < Res.info_size(); i++)
	{

		Message::Game_bulletinfo_Response_Info info = Res.info(i);
		Tank* ptank = Tank_info[info.tankid()];
		if (ptank == ptank1) continue;
		bullet* newhead = new bullet();
		bullet* temp = newhead;

		for (int i = 0; i < info.bulletinfo_size(); i++)
		{
			Message::bulletinfo binfo = info.bulletinfo(i);

			temp->next = new bullet(binfo.locationx(), binfo.locationy(), binfo.rotate(), (BulletStyle)binfo.bullet_style());
			temp = temp->next;
		}
		ptank->bullet_head = newhead->next;
		delete(newhead);
	}


	//to_destroyed_bulletinfo.push(optank->bullet_head);
	//cv.notify_one();
}

void Game::recv_hitbrick(Header& header, char* content)
{
	Message::Game_brick_hited_Response Res;
	Res.ParseFromArray(content, header.length);

	int hited_brick_id = Res.hited_brick_id();
	int health = Res.health();

	auto iter = map_info.Component_info.begin();
	while (iter != map_info.Component_info.end())
	{
		if ((*iter)->type == component_type::BRICK && (*iter)->id == hited_brick_id)
			break;
		iter++;
	}
	if (iter == map_info.Component_info.end())
		return;

	(*iter)->set_health(health);
	LOGINFO("Hit_brick ID:{},Health:{}", hited_brick_id, health);
	if (health <= 0)
	{
		LOGINFO("recv_hitbrick Break ID:{}", hited_brick_id);
		GameSceneManager::Instance()->remove(*iter);
		map_info.Component_info.erase(iter);
	}
}

void Game::recv_hited(Header& header, char* content)
{
	Message::Game_tank_hited_Response Res;
	Res.ParseFromArray(content, header.length);

	int hited_id = Res.hited_tank_id();
	if (Tank_info[hited_id])
		Tank_info[hited_id]->health = Res.health();
}

void Game::recv_myhited()
{
	ptank1->health -= 21;
}

void Game::recv_destoryed(Header& header, char* content)
{
	Message::Game_destroyed_tank_Response Res;
	Res.ParseFromArray(content, header.length);

	int hited_id = Res.destroyed_tank_id();

	if (Tank_info[hited_id])
		Tank_info[hited_id]->destory();
}

void Game::recv_mydestoryed()
{
	ptank1->isalive = false;
}

void set_My_id(int id)
{
	::my_tank_location = id;
}

void Game::Tank_Input()
{
	if (GetFocus() != _hwnd)
		return;

	if (ptank1->isalive)
	{
		if (GetAsyncKeyState(key1[0]) & 0x8000)
			Tank_Move(ptank1, true);
		else if (GetAsyncKeyState(key1[1]) & 0x8000)
			Tank_Move(ptank1, false);

		if (GetAsyncKeyState(key1[2]) & 0x8000)
			Tank_Rotate(ptank1, false);
		else if (GetAsyncKeyState(key1[3]) & 0x8000)
			Tank_Rotate(ptank1, true);

		if (GetAsyncKeyState(key1[4]) & 0x8000)
			Tank_shot(ptank1);
	}

	if (!isonline_game && ptank2->isalive)
	{
		if (GetAsyncKeyState(key2[0]) & 0x8000)
			Tank_Move(ptank2, true);
		else if (GetAsyncKeyState(key2[1]) & 0x8000)
			Tank_Move(ptank2, false);

		if (GetAsyncKeyState(key2[2]) & 0x8000)
			Tank_Rotate(ptank2, false);
		else if (GetAsyncKeyState(key2[3]) & 0x8000)
			Tank_Rotate(ptank2, true);

		if (GetAsyncKeyState(key2[4]) & 0x8000)
			Tank_shot(ptank2);
	}
}

void Game::Tank_shot(Tank* ptank) {
	if (ptank->bullet_count < ptank->bullet_limited)
	{
		ptank->bullet_now = clock();
		if (ptank->bullet_now - ptank->bullet_last > 250)
		{
			ptank->Addbullet(BulletStyle::DEFAULT);
			ptank->bullet_count++;
			ptank->bullet_last = ptank->bullet_now;
		}
	}
}

void Game::Tank_Move(Tank* ptank, bool forward)
{

	double ori_locationX = ptank->get_locationX();
	double ori_locationY = ptank->get_locationY();

	if (forward)
	{
		ptank->set_locationX(ori_locationX + sin(ptank->get_rotate() * M_PI / 180) * ptank->speed);
		ptank->set_locationY(ori_locationY - cos(ptank->get_rotate() * M_PI / 180) * ptank->speed);
	}
	else {
		ptank->set_locationX(ptank->get_locationX() - sin(ptank->get_rotate() * M_PI / 180) * ptank->speed);
		ptank->set_locationY(ptank->get_locationY() + cos(ptank->get_rotate() * M_PI / 180) * ptank->speed);
	}

	bool isreturn = false;

	for (auto& v : Tank_info)
	{
		if (v.second == ptank || v.second->isalive == false)
			continue;
		Tank* pother_tank = v.second;
		if (ptank->collision(*pother_tank))
		{
			ptank->set_locationX(ori_locationX);
			ptank->set_locationY(ori_locationY);
			isreturn = true;
			break;
		}
	}

	set<Game_Component* >collision_set;
	GameSceneManager::Instance()->Quary_Collision(ptank, collision_set);
	vector<Game_Component*> Wall_Com;
	vector<Game_Component*> Prop_Com;
	for (auto pCom : collision_set)
	{
		if (pCom->type >= component_type::PROP_DEFAULT)
			Prop_Com.push_back(pCom);
		else
			Wall_Com.push_back(pCom);
	}

	if (!isreturn)
	{
		for (auto& pCom : Wall_Com)
		{
			if (ptank->collision(*pCom))
			{
				ptank->set_locationX(ori_locationX);
				ptank->set_locationY(ori_locationY);
				isreturn = true;
				break;
			}
		}
	}

	for (auto& pCom : Prop_Com)
	{
		Prop* pProp = dynamic_cast<Prop*>(pCom);
		if (!pProp)
			break;
		if (ptank->collision(*pProp))
		{
			if (isonline_game)
			{
				int id = -1;
				for (auto& v : Tank_info) {
					if (v.second == ptank)
					{
						id = v.first;
						break;
					}
				}
				pProp->online_get(id);
				break;
			}
			pProp->get(ptank);
		}
	}

	if (ptank->collision_broader())
	{
		ptank->set_locationX(ori_locationX);
		ptank->set_locationY(ori_locationY);
		return;
	}
}

void Game::Tank_Rotate(Tank* ptank, bool forward)
{
	double ori_rotate = ptank->get_rotate();

	if (forward)
	{
		ptank->set_rotate(ori_rotate + 5.0);
	}
	else {
		ptank->set_rotate(ori_rotate - 5.0);
	}

	bool isreturn = false;

	for (auto& v : Tank_info)
	{
		if (v.second == ptank || v.second->isalive == false)
			continue;
		Tank* pother_tank = v.second;
		if (ptank->collision(*pother_tank))
		{
			ptank->set_rotate(ori_rotate);
			isreturn = true;
			break;
		}
	}

	set<Game_Component* >collision_set;
	GameSceneManager::Instance()->Quary_Collision(ptank, collision_set);
	vector<Game_Component*> Wall_Com;
	vector<Game_Component*> Prop_Com;
	for (auto pCom : collision_set)
	{
		if (pCom->type >= component_type::PROP_DEFAULT)
			Prop_Com.push_back(pCom);
		else
			Wall_Com.push_back(pCom);
	}

	if (!isreturn)
	{
		for (auto& pCom : Wall_Com)
		{
			if (ptank->collision(*pCom))
			{
				ptank->set_rotate(ori_rotate);
				isreturn = true;
				break;
			}
		}
	}

	for (auto& pCom : Prop_Com)
	{
		Prop* pProp = dynamic_cast<Prop*>(pCom);
		if (!pProp)
			break;
		if (ptank->collision(*pProp))
		{
			if (isonline_game)
			{
				int id = -1;
				for (auto& v : Tank_info) {
					if (v.second == ptank)
					{
						id = v.first;
						break;
					}
				}
				pProp->online_get(id);
				break;
			}
			pProp->get(ptank);
		}
	}

	if (ptank->collision_broader())
	{
		ptank->set_rotate(ori_rotate);
		return;
	}
}

void Game::AI_Rotate(AI_control* AI, bool forward) {
	Tank* ptank = AI->AI_Tank;

	double ori_rotate = ptank->get_rotate();

	if (forward)
	{
		ptank->set_rotate(ori_rotate + 5.0);
	}
	else {
		ptank->set_rotate(ori_rotate - 5.0);
	}

	bool isreturn = false;

	for (auto& v : Tank_info)
	{
		if (v.second == ptank || v.second->isalive == false)
			continue;
		Tank* pother_tank = v.second;
		if (ptank->collision(*pother_tank))
		{
			ptank->set_rotate(ori_rotate);
			isreturn = true;
			break;
		}
	}

	set<Game_Component* >collision_set;
	GameSceneManager::Instance()->Quary_Collision(ptank, collision_set);
	vector<Game_Component*> Wall_Com;
	vector<Game_Component*> Prop_Com;
	for (auto pCom : collision_set)
	{
		if (pCom->type >= component_type::PROP_DEFAULT)
			Prop_Com.push_back(pCom);
		else
			Wall_Com.push_back(pCom);
	}

	if (!isreturn)
	{
		for (auto& pCom : Wall_Com)
		{
			if (ptank->collision(*pCom))
			{
				ptank->set_rotate(ori_rotate);
				isreturn = true;
				break;
			}
		}
	}

	for (auto& pCom : Prop_Com)
	{
		Prop* pProp = dynamic_cast<Prop*>(pCom);
		if (!pProp)
			break;
		if (ptank->collision(*pProp))
		{
			if (isonline_game)
			{
				int id = -1;
				for (auto& v : Tank_info) {
					if (v.second == ptank)
					{
						id = v.first;
						break;
					}
				}
				pProp->online_get(id);
				break;
			}
			pProp->get(ptank);
		}
	}

	if (ptank->collision_broader())
	{
		ptank->set_rotate(ori_rotate);
		return;
	}
}

void Game::Bullet_Tick_Move(bullet* pbullet)
{
	if (!pbullet) return;

	bool result = false;
	Pos ori_location(pbullet->get_locationX(), pbullet->get_locationY());
	pbullet->set_locationX(ori_location.x + sin(pbullet->get_rotate() * M_PI / 180) * pbullet->speed);
	pbullet->set_locationY(ori_location.y - cos(pbullet->get_rotate() * M_PI / 180) * pbullet->speed);

	for (auto& v : Tank_info)
	{
		if (v.second == pbullet->owner || v.second->isalive == false)
			continue;
		Tank* pother_tank = v.second;

		if (pbullet->collision(*pother_tank))
		{
			if (isonline_game)
			{
				send_hittank(v.first, pbullet);
				pbullet->destroy();
				result = true;
				break;
			}
			// else{ 
			pother_tank->health -= 21;
			if (pother_tank->health <= 0)
			{
				pother_tank->destory();
			}
			pbullet->destroy();
			player_alive--;
			result = true;
			break;
			//}
		}
	}

	if (result == false)
	{
		set<Game_Component* >collision_set;
		GameSceneManager::Instance()->Quary_Collision(pbullet, collision_set);
		vector<Game_Component*> Wall_Com;
		for (auto pCom : collision_set)
		{
			if (pCom->type < component_type::PROP_DEFAULT)
				Wall_Com.push_back(pCom);
		}

		struct Collision_info {
			Game_Component* com;
			Collision_state state;
			Collision_info(Game_Component* com, Collision_state state)
				:com(com), state(state) {}

		};

		vector<Collision_info> collision_map;
		for (auto& pCom : Wall_Com)
		{
			Collision_state collision_state;
			bool collision_result = pbullet->collision(*pCom, ori_location, &collision_state);

			if (collision_result)
			{
				collision_map.emplace_back(pCom, collision_state);
			}
		}

		if (!collision_map.empty())
		{
			Game_Component* pCom = nullptr;
			Collision_state collision_state;
			double min_distance = DBL_MAX;
			for (auto& it : collision_map)
			{
				double cur_distance = distance(it.state.pos, ori_location);
				if (cur_distance < min_distance)
				{
					pCom = it.com;
					collision_state = it.state;
					min_distance = cur_distance;
				}
			}

			switch (pCom->type)
			{
			case component_type::BRICK:
			{
				if (pbullet->destroy() == false)break;

				if (isonline_game)
				{
					send_hitbrick(pCom->id, pbullet);
					result = true;
					break;
				}
				else
				{
					int health = pCom->reduce_health(21);
					if (health <= 0)
					{
						for (auto it = map_info.Component_info.begin(); it != map_info.Component_info.end(); it++)
						{
							if (*it == pCom)
							{
								map_info.Component_info.erase(it);
								break;
							}
						}
						GameSceneManager::Instance()->remove(pCom);
					}
					result = true;
					break;
				}
			}
			case component_type::IRON:
			{

				if (pbullet->isBounce && pCom->id == pbullet->last_bounceId)
				{
					result = true;
					break;
				}
				if (pbullet->BounceCount >= 5)
				{
					pbullet->destroy();
				}
				else {
					pbullet->isBounce = true;
					pbullet->last_bounceId = pCom->id;

					pbullet->set_locationX(collision_state.pos.x);
					pbullet->set_locationY(collision_state.pos.y);
					pbullet->set_rotate(collision_state.rotate);

					pbullet->BounceCount++;
				}
				result = true;
				break;
			}
			default:
				break;
			}
		}
	}

	if (result == false)
		pbullet->isBounce = false;

	if (result == false && pbullet->collision_broader())
	{
		pbullet->destroy();
	}

	Bullet_Tick_Move(pbullet->next);
}

D2D_GIF* Game::AddGIF(int x1, int y1, int x2, int y2, GIFINFO* gifInfo, int loopCount)
{
	D2D_GIF* Gif = new D2D_GIF(x1, y1, x2, y2, gifInfo, loopCount);
	Gif_list.emplace_back(Gif);
	return Gif;
}
