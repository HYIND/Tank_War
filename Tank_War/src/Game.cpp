#include "Game.h"
#include "collision.h"
#include "keymap.h"

#define _USE_MATH_DEFINES
#include <math.h>

extern SOCKET tcp_socket;

Game* Cur_Game = new Game();
int my_tank_location = 1;

extern RECT _rect;

void Game::Init_Game(int map_id, int my_id)
{
	Get_keymap();
	this->map_info = Map_list[map_id];
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
	this->my_tankid = my_id;
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

void Game::Move()
{
	if (!isonline_game)
	{
		for (auto& v : Tank_info)
		{
			if (v.second->bullet_head)
				Bullet_Move(v.second->bullet_head);
		}
	}
	else
	{
		if (ptank1->bullet_head)
			Bullet_Move(ptank1->bullet_head);
	}
}

void Game::Draw()
{
	if (map_info.BK_pBitmap)
		pRenderTarget->DrawBitmap(map_info.BK_pBitmap, D2D1::RectF(0, 0, _rect.right, _rect.bottom));;
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
	binfo->set_locationx(pbullet->locationX);
	binfo->set_locationy(pbullet->locationY);
	binfo->set_rotate(pbullet->rotate);
	binfo->set_bullet_style((int)pbullet->bullet_style);
	Req.set_allocated_bulletinfo(binfo);

	Send(Req);
}

void Game::send_hitbrick(int id, bullet* pbullet)
{
  	Message::Game_hit_brick_Request Req;
	Req.set_hited_brick_id(id);

	Message::bulletinfo* binfo = new Message::bulletinfo();
	binfo->set_locationx(pbullet->locationX);
	binfo->set_locationy(pbullet->locationY);
	binfo->set_rotate(pbullet->rotate);
	binfo->set_bullet_style((int)pbullet->bullet_style);
	Req.set_allocated_bulletinfo(binfo);

	Send(Req);
}

void Game::send_mytankinfo()
{
	Message::Game_tankinfo_Request Req;
	Req.set_locationx(ptank1->locationX);
	Req.set_locationy(ptank1->locationY);
	Req.set_rotate(ptank1->rotate);
	Req.set_tank_style((int)ptank1->tank_style);

	Send(Req);
}

void Game::send_bullet()
{
	bullet* cur = ptank1->bullet_head;
	Message::Game_bulletinfo_Request Req;
	while (cur != NULL)
	{
		Message::bulletinfo* bulletinfo = Req.add_bulletinfo();
		bulletinfo->set_locationx(cur->locationX);
		bulletinfo->set_locationy(cur->locationY);
		bulletinfo->set_rotate(cur->rotate);
		bulletinfo->set_bullet_style((int)cur->bullet_style);
		cur = cur->next;
	}
	Send(Req);
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
		ptank->locationX = info->locationx();
		ptank->locationY = info->locationy();
		ptank->rotate = info->rotate();
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

	if (health <= 0)
	{
		map_info.Component_info.erase(iter);
		return;
	}
	(*iter)->health = health;
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
		Tank_info[hited_id]->isalive = false;
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
	if (GetAsyncKeyState(key1[0]) & 0x8000)
	{
		Tank_Move(ptank1, true);
	}
	else if (GetAsyncKeyState(key1[1]) & 0x8000)
	{
		Tank_Move(ptank1, false);
	}
	if (GetAsyncKeyState(key1[2]) & 0x8000)
	{
		Tank_Rotate(ptank1, false);
	}
	else if (GetAsyncKeyState(key1[3]) & 0x8000)
	{
		Tank_Rotate(ptank1, true);
	}

	if (GetAsyncKeyState(key1[4]) & 0x8000)
	{
		Tank_shot(ptank1);
	}
	if (!isonline_game)
	{
		if (GetAsyncKeyState(key2[0]) & 0x8000)
		{
			Tank_Move(ptank2, true);
		}
		else if (GetAsyncKeyState(key2[1]) & 0x8000)
		{

			Tank_Move(ptank2, false);

		}
		if (GetAsyncKeyState(key2[2]) & 0x8000)
		{
			Tank_Rotate(ptank2, false);
		}
		else if (GetAsyncKeyState(key2[3]) & 0x8000)
		{
			Tank_Rotate(ptank2, true);
		}
		if (GetAsyncKeyState(key2[4]) & 0x8000)
		{
			Tank_shot(ptank2);
		}
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
	double new_locationX = ptank->locationX;
	double new_locationY = ptank->locationY;

	if (forward)
	{
		new_locationX += sin(ptank->rotate * M_PI / 180) * ptank->speed;
		new_locationY -= cos(ptank->rotate * M_PI / 180) * ptank->speed;
	}
	else {
		new_locationX -= sin(ptank->rotate * M_PI / 180) * ptank->speed;
		new_locationY += cos(ptank->rotate * M_PI / 180) * ptank->speed;
	}

	for (auto& v : Tank_info)
	{
		if (v.second == ptank || v.second->isalive == false)
			continue;
		Tank* pother_tank = v.second;
		if (collision_obb(
			new_locationX, new_locationY, ptank->width, ptank->height, ptank->rotate,
			pother_tank->locationX, pother_tank->locationY, pother_tank->width, pother_tank->height, pother_tank->rotate))
			return;
	}

	for (auto& v : map_info.Component_info)
	{
		if (collision_obb(
			new_locationX, new_locationY, ptank->width, ptank->height, ptank->rotate,
			v->locationX, v->locationY, v->width, v->height)
			)
			return;
	}

	for (auto it = Prop_info.begin(); it != Prop_info.end(); it++) {
		Prop* pProp = it->second;
		if (collision_obb(
			new_locationX, new_locationY, ptank->width, ptank->height, ptank->rotate,
			pProp->locationX, pProp->locationY, pProp->width, pProp->height)
			)
		{
			if (isonline_game) {
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

	if (collision_broader(new_locationX, new_locationY, ptank->width, ptank->height, ptank->rotate)) return;

	ptank->locationY = new_locationY;
	ptank->locationX = new_locationX;
}

void Game::Tank_Rotate(Tank* ptank, bool forward)
{
	double new_rotate = ptank->rotate;

	if (forward)
	{
		new_rotate += 5;
	}
	else {
		new_rotate -= 5;
	}

	for (auto& v : Tank_info)
	{
		if (v.second == ptank || v.second->isalive == false)
			continue;
		Tank* pother_tank = v.second;
		if (collision_obb(
			ptank->locationX, ptank->locationY, ptank->width, ptank->height, new_rotate,
			pother_tank->locationX, pother_tank->locationY, pother_tank->width, pother_tank->height, pother_tank->rotate))
			return;
	}

	for (auto& v : map_info.Component_info)
	{
		if (collision_obb(
			ptank->locationX, ptank->locationY, ptank->width, ptank->height, new_rotate,
			v->locationX, v->locationY, v->width, v->height))
			return;
	}

	if (collision_broader(ptank->locationX, ptank->locationY, ptank->width, ptank->height, new_rotate)) return;

	ptank->rotate = new_rotate;
}

void Game::AI_Rotate(AI_control* AI, bool forward) {
	Tank* ptank = AI->AI_Tank;
	double new_rotate = ptank->rotate;

	if (forward)
	{
		new_rotate += min(5, AI->goal_rotate - new_rotate);
	}
	else {
		new_rotate -= min(5, new_rotate - AI->goal_rotate);
	}

	for (auto& v : Tank_info)
	{
		if (v.second == ptank)
			continue;
		Tank* pother_tank = v.second;
		if (collision_obb(
			ptank->locationX, ptank->locationY, ptank->width, ptank->height, new_rotate,
			pother_tank->locationX, pother_tank->locationY, pother_tank->width, pother_tank->height, pother_tank->rotate))
			return;
	}

	for (auto& v : map_info.Component_info)
	{
		if (collision_obb(
			ptank->locationX, ptank->locationY, ptank->width, ptank->height, new_rotate,
			v->locationX, v->locationY, v->width, v->height))
			return;
	}

	if (collision_broader(ptank->locationX, ptank->locationY, ptank->width, ptank->height, new_rotate)) return;

	ptank->rotate = new_rotate;
}

struct collisioninfo
{
	Game_Component* Component = nullptr;
	Collision_Pos collision_result = NullPos;
	double distance = 0.0;
	collisioninfo(Game_Component* Component_in, Collision_Pos collision_result_in, double distance_in) :
		Component(Component_in), collision_result(collision_result_in), distance(distance_in) {}
};
void Game::Bullet_Move(bullet* pbullet)
{
	if (!pbullet) return;

	bool result = false;
	pbullet->locationX += sin(pbullet->rotate * M_PI / 180) * pbullet->speed;
	pbullet->locationY -= cos(pbullet->rotate * M_PI / 180) * pbullet->speed;

	for (auto& v : Tank_info)
	{
		if (v.second == pbullet->owner || v.second->isalive == false)
			continue;
		Tank* pother_tank = v.second;

		if (collision_obb(
			pbullet->locationX, pbullet->locationY, pbullet->width, pbullet->height, pbullet->rotate,
			pother_tank->locationX, pother_tank->locationY, pother_tank->width, pother_tank->height, pother_tank->rotate))
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
				pother_tank->isalive = false;
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
		vector<collisioninfo> collision_map;
		for (auto it = map_info.Component_info.begin(); it != map_info.Component_info.end(); it++)
		{
			Game_Component* pWall = (*it);
			double distance = 0.0;
			Collision_Pos collision_result = collision_round_aabb_withInfo(
				pbullet->locationX, pbullet->locationY, pbullet->width / 2, pbullet->rotate,
				pWall->locationX, pWall->locationY, pWall->width, pWall->height, 90.0, &distance);

			if (collision_result != 0)
			{
				collision_map.emplace_back(pWall, collision_result, distance);
			}
		}

		if (!collision_map.empty())
		{
			Game_Component* pWall = nullptr;
			Collision_Pos collision_result = NullPos;
			double min_distance = DBL_MAX;
			for (auto& it : collision_map)
			{
				if (it.distance < min_distance)
				{
					pWall = it.Component;
					collision_result = it.collision_result;
				}
			}
			switch (pWall->type)
			{
			case component_type::BRICK:
			{
				if (pbullet->destroy() == false)break;

				if (isonline_game)
				{
					send_hitbrick(pWall->id, pbullet);
					result = true;
					break;
				}
				else
				{
					pWall->health -= 21;
					if (pWall->health <= 0)
					{
						for (auto it = map_info.Component_info.begin(); it != map_info.Component_info.end(); it++)
						{
							if (*it == pWall)
							{
								map_info.Component_info.erase(it);
								break;
							}
						}
					}
					result = true;
					break;
				}
			}
			case component_type::IRON:
			{
				if (pbullet->isBounce && pWall->id == pbullet->last_bounceId)
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
					pbullet->last_bounceId = pWall->id;
					double axis_rotate = 0.0;		//计算反弹时入射角和出射角的对称轴，取值与撞击点和被撞矩形的角度相关
					switch (collision_result)
					{
					case LeftTopCorner:
					case RightBottomCorner:
					{
						axis_rotate = 45.0;
						break;
					}
					case RightTopCorner:
					case LeftBottomCorner:
					{
						axis_rotate = 135.0;
						break;
					}
					case LeftBroader:
					case RightBroader:
					{
						axis_rotate = 0.0;
						break;
					}
					case TopBroader:
					case BottomBroader:
					{
						axis_rotate = 90.0;
						break;
					}
					default:
						break;
					}
					double format_rotate = fmod(pbullet->rotate, 360.0);
					format_rotate = 2.0 * axis_rotate - format_rotate;
					format_rotate = fmod(format_rotate, 360.0);
					pbullet->rotate = format_rotate;
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

	if (result == false && collision_broader(pbullet->locationX, pbullet->locationY, pbullet->width, pbullet->height, pbullet->rotate))
	{
		pbullet->destroy();
	}

	Bullet_Move(pbullet->next);
}
