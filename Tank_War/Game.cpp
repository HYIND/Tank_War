#include "Game.h"
#include "collision.h"
#include "keymap.h"


extern SOCKET mysocket;

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
			v.direction,
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

void Game::Tank_Input()
{
	if (GetAsyncKeyState(key1[0]) & 0x8000)
	{
		if (ptank1->direction != UP)
		{
			ptank1->direction = UP;
		}
		else
		{
			Tank_Move(ptank1);
		}
	}
	else if (GetAsyncKeyState(key1[2]) & 0x8000)
	{
		if (ptank1->direction != LEFT)
		{
			ptank1->direction = LEFT;
		}
		else
		{
			Tank_Move(ptank1);
		}
	}
	else if (GetAsyncKeyState(key1[1]) & 0x8000)
	{
		if (ptank1->direction != DOWN)
		{
			ptank1->direction = DOWN;
		}
		else
		{
			Tank_Move(ptank1);
		}
	}
	else if (GetAsyncKeyState(key1[3]) & 0x8000)
	{
		if (ptank1->direction != RIGHT)
		{
			ptank1->direction = RIGHT;
		}
		else
		{
			Tank_Move(ptank1);
		}
	}
	if (GetAsyncKeyState(key1[4]) & 0x8000)
	{
		if (ptank1->bullet_count < ptank1->bullet_limited)
		{
			ptank1->bullet_now = clock();
			if (ptank1->bullet_now - ptank1->bullet_last > 250)
			{
				ptank1->Addbullet(BulletStyle::DEFAULT);
				ptank1->bullet_count++;
				ptank1->bullet_last = ptank1->bullet_now;
			}
		}
	}
	if (!isonline_game)
	{
		if (GetAsyncKeyState(key2[0]) & 0x8000)
		{
			if (ptank2->direction != UP)
			{
				ptank2->direction = UP;
			}
			else
			{
				Tank_Move(ptank2);
			}
		}
		else if (GetAsyncKeyState(key2[2]) & 0x8000)
		{
			if (ptank2->direction != LEFT)
			{
				ptank2->direction = LEFT;
			}
			else
			{
				Tank_Move(ptank2);
			}
		}
		else if (GetAsyncKeyState(key2[1]) & 0x8000)
		{
			if (ptank2->direction != DOWN)
			{
				ptank2->direction = DOWN;
			}
			else
			{
				Tank_Move(ptank2);
			}
		}
		else if (GetAsyncKeyState(key2[3]) & 0x8000)
		{
			if (ptank2->direction != RIGHT)
			{
				ptank2->direction = RIGHT;
			}
			else
			{
				Tank_Move(ptank2);
			}
		}
		if (GetAsyncKeyState(key2[4]) & 0x8000)
		{
			if (ptank2->bullet_count < ptank2->bullet_limited)
			{
				ptank2->bullet_now = clock();
				if (ptank2->bullet_now - ptank2->bullet_last > 250)
				{
					ptank2->Addbullet(BulletStyle::DEFAULT);
					ptank2->bullet_count++;
					ptank2->bullet_last = ptank2->bullet_now;
				}
			}
		}
	}
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

void Game::Tank_Move(Tank* ptank)
{
	int new_locationX = ptank->locationX;
	int new_locationY = ptank->locationY;
	switch (ptank->direction)
	{
	case UP:
		new_locationY -= ptank->speed;
		break;
	case DOWN:
		new_locationY += ptank->speed;
		break;
	case LEFT:
		new_locationX -= ptank->speed;
		break;
	case RIGHT:
		new_locationX += ptank->speed;
		break;
	}
	for (auto& v : Tank_info)
	{
		if (v.second == ptank)
			continue;
		Tank* pother_tank = v.second;
		Object_Check_predict(ptank, pother_tank);
	}

	for (auto& v : map_info.Brick_info)
	{
		Brick_Wall* pBWall = &v;
		Object_Check_predict(ptank, pBWall);
	}
	for (auto& v : map_info.Iron_info)
	{
		Iron_Wall* pIWall = &v;
		Object_Check_predict(ptank, pIWall);
	}


	if (new_locationX - ptank->width / 2 < _rect.left)
	{
		ptank->locationX = _rect.left + ptank->width / 2;
		return;
	}
	if (new_locationX + ptank->width / 2 > _rect.right)
	{
		ptank->locationX = _rect.right - ptank->width / 2;
		return;
	}
	if (new_locationY - ptank->height / 2 < _rect.top)
	{
		ptank->locationY = _rect.top + ptank->height / 2;
		return;
	}
	if (new_locationY + ptank->height / 2 > _rect.bottom)
	{
		ptank->locationY = _rect.bottom - ptank->height / 2;
		return;
	}

	ptank->locationY = new_locationY;
	ptank->locationX = new_locationX;
}

void Game::Bullet_Move(bullet* pbullet)
{
	if (!pbullet) return;
	int new_locationX = pbullet->locationX;
	int new_locationY = pbullet->locationY;
	switch (pbullet->direction)
	{
	case UP:
		new_locationY -= pbullet->speed;
		break;
	case DOWN:
		new_locationY += pbullet->speed;
		break;
	case LEFT:
		new_locationX -= pbullet->speed;
		break;
	case RIGHT:
		new_locationX += pbullet->speed;
		break;
	}
	pbullet->locationX = new_locationX;
	pbullet->locationY = new_locationY;
	for (auto& v : Tank_info)
	{
		if (v.second == pbullet->owner || v.second->isalive == false)
			continue;
		Tank* pother_tank = v.second;
		if (Object_Check(pbullet, pother_tank))
		{
			if (isonline_game)
			{
				send_hittank(v.first, pbullet);
				pbullet->destroy();
				return;
			}
			pother_tank->health -= 21;
			if (pother_tank->health <= 0)
			{
				pother_tank->isalive = false;
			}
			pbullet->destroy();
			player_alive--;
			return;
		}
	}
	for (auto it = map_info.Brick_info.begin(); it != map_info.Brick_info.end(); it++)
	{
		Brick_Wall* pBWall = &(*it);
		if (Object_Check(pbullet, pBWall))
		{
			if (isonline_game)
			{
				send_hitbrick(pBWall->id, pbullet);
				pbullet->destroy();
				return;
			}
			pbullet->destroy();
			pBWall->health -= 21;
			if (pBWall->health <= 0)
			{
				map_info.Brick_info.erase(it);
			}
			return;
		}
	}

	for (auto it = map_info.Iron_info.begin(); it != map_info.Iron_info.end(); it++)
	{
		Iron_Wall* pIWall = &(*it);
		if (Object_Check(pbullet, pIWall))
		{
			pbullet->destroy();
			return;
		}
	}

	if (pbullet->locationX - pbullet->width / 2 > _rect.right || pbullet->locationX + pbullet->width / 2 < _rect.left
		|| pbullet->locationY - pbullet->height / 2 > _rect.bottom || pbullet->locationY + pbullet->height / 2 < _rect.top)
	{
		pbullet->destroy();
	}
	Bullet_Move(pbullet->next);
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
	binfo->set_direction(pbullet->direction);
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
	binfo->set_direction(pbullet->direction);
	binfo->set_bullet_style((int)pbullet->bullet_style);
	Req.set_allocated_bulletinfo(binfo);

	Send(Req);
}

void Game::send_mytankinfo()
{
	Message::Game_tankinfo_Request Req;
	Req.set_locationx(ptank1->locationX);
	Req.set_locationy(ptank1->locationY);
	Req.set_direction(ptank1->direction);
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
		bulletinfo->set_direction(cur->direction);
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
		ptank->direction = info->direction();
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

			temp->next = new bullet(binfo.locationx(), binfo.locationy(), binfo.direction(), (BulletStyle)binfo.bullet_style());
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

	auto iter = map_info.Brick_info.begin();
	while (iter != map_info.Brick_info.end())
	{
		if (iter->id == hited_brick_id)
			break;
		iter++;
	}
	if (iter == map_info.Brick_info.end())
		return;

	if (health <= 0)
	{
		map_info.Brick_info.erase(iter);
		return;
	}
	Brick_Wall* pwall = &(*iter);
	pwall->health = health;
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
