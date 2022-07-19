#include "Game.h"

#define  Object_Check(A,B) collision(A->locationX, A->locationY,A->width, A->height,B->locationX, B->locationY,B->width, B->height)

#define  Object_Check_predict(A,B) if(collision(new_locationX, new_locationY,A->width, A->height,B->locationX, B->locationY,B->width, B->height))\
		{\
		switch (ptank->direction)\
		{\
		case UP:\
			new_locationY = B->locationY + B->height / 2 + A->height / 2;\
			break;\
		case DOWN:\
			new_locationY = B->locationY - B->height / 2 - A->height / 2;\
			break;\
		case LEFT:\
			new_locationX = B->locationX + B->width / 2 + A->width / 2;\
			break;\
		case RIGHT:\
			new_locationX = B->locationX - B->width / 2 - A->width / 2;\
			break;\
		}\
		A->locationX = new_locationX;\
		A->locationY = new_locationY;\
		return;\
		}\

extern SOCKET mysocket;

Game* Cur_Game = new Game();
int my_tank_location = 1;

extern RECT _rect;

bool collision(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
	//¼ì²âYÖáÅö×²,Åö×²Ôò·µ»Øtrue;
	bool collisionY = y1 - height1 / 2 < y2 + height2 / 2 == y1 + height1 / 2 > y2 - height2 / 2;
	//¼ì²âXÖáÅö×²,Åö×²Ôò·µ»Øtrue
	bool collisionX = x1 - width1 / 2 < x2 + width2 / 2 == x1 + width1 / 2 > x2 - width2 / 2;

	if (collisionX && collisionY)
	{
		return true;
	}
	return false;
}

void Game::Init_Game(int map_id, int my_id)
{
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

void Game::Tank_Input()
{
	if (GetAsyncKeyState('W') & 0x8000)
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
	else if (GetAsyncKeyState('A') & 0x8000)
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
	else if (GetAsyncKeyState('S') & 0x8000)
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
	else if (GetAsyncKeyState('D') & 0x8000)
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
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
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
		if (GetAsyncKeyState(VK_UP) & 0x8000)
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
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
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
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
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
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
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
		if (GetAsyncKeyState(VK_OEM_2) & 0x8000)
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
	Tank_Input();
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
		//if (Object_Check(ptank, pother_tank))
		//{
		//	switch (ptank->direction)
		//	{
		//	case UP:
		//		new_locationY = pother_tank->locationY + pother_tank->height / 2 + ptank->height / 2;
		//		break;
		//	case DOWN:
		//		new_locationY = pother_tank->locationY - pother_tank->height / 2 - ptank->height / 2;
		//		break;
		//	case LEFT:
		//		new_locationX = pother_tank->locationX + pother_tank->width / 2 + ptank->width / 2;
		//		break;
		//	case RIGHT:
		//		new_locationX = pother_tank->locationX - pother_tank->width / 2 - ptank->width / 2;
		//		break;
		//	}
		//	ptank->locationY = new_locationY;
		//	ptank->locationX = new_locationX;
		//	return;
		//}
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

// destorytank:(INT){INT,INT,bulletStyle}
void Game::send_hit(int id, bullet* pbullet)
{
	char ch[1024] = "hittank:";

	int cur_loc = 8;
	ch[cur_loc] = '(';
	cur_loc++;
	memcpy(&ch[cur_loc], &id, sizeof(int));
	cur_loc += sizeof(int);
	ch[cur_loc] = ')';
	cur_loc++;
	ch[cur_loc] = '{';
	cur_loc++;
	memcpy(&ch[cur_loc], &(pbullet->locationX), sizeof(int) * 3 + sizeof(BulletStyle));
	cur_loc += sizeof(int) * 3 + sizeof(BulletStyle);
	ch[cur_loc] = '}';

	send(mysocket, (const char*)&ch[0], 1023, 0);
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
				send_hit(v.first, pbullet);
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
			pbullet->destroy();
			pBWall->health -= 35;
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
			pIWall->health -= 35;
			if (pIWall->health <= 0)
			{
				map_info.Iron_info.erase(it);
			}
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

void Game::send_mytankinfo()
{
	char buffer[1024] = "mytankinfo:";
	int i = sizeof(Tank);
	memcpy(&buffer[11], ptank1, sizeof(Tank));
	send(mysocket, buffer, 1023, 0);
}
//mybullet:{xystyle} {}
void Game::send_bullet()
{
	bullet* cur = ptank1->bullet_head;
	char ch[1024] = "mybullet:";
	int cur_loc = 9;
	while (cur != NULL)
	{
		if (cur_loc > 900)
			break;

		ch[cur_loc] = '{';
		cur_loc++;
		memcpy(&ch[cur_loc], &(cur->locationX), sizeof(int) * 3 + sizeof(BulletStyle));
		cur_loc += sizeof(int) * 3 + sizeof(BulletStyle);
		ch[cur_loc] = '}';
		cur_loc++;
		cur = cur->next;
	}
	send(mysocket, (const char*)&ch[0], 1023, 0);
}

//tankinfo:(INT){22}
void Game::refrash_tankinfo(char ch[])
{
	int id = 0;
	int cur_loc = 9;
	int i = sizeof(Tank);
	if (ch[cur_loc] == '(' && ch[cur_loc + sizeof(int) + 1] == ')')
	{
		cur_loc++;
		memcpy(&id, &ch[cur_loc], sizeof(int));
		cur_loc = cur_loc + sizeof(int) + 1;
	}
	else return;
	if (ch[cur_loc] == '{' && ch[cur_loc + 21 + 1] == '}')
	{
		cur_loc++;
		memcpy(Tank_info[id], &ch[cur_loc], 21);
		cur_loc = cur_loc + 21;
	}
	else return;
}
//opbulletinfo:(int){xystyle}
void Game::refrash_bullet(char ch[])
{
	bullet* newhead = new bullet();
	bullet* temp = newhead;

	int id = 0;
	int cur_loc = 11;
	if (ch[cur_loc] == '(' && ch[cur_loc + sizeof(int) + 1] == ')')
	{
		cur_loc++;
		memcpy(&id, &ch[cur_loc], sizeof(int));
		cur_loc = cur_loc + sizeof(int) + 1;
	}
	else return;
	while (ch[cur_loc] == '{' && ch[cur_loc + sizeof(int) * 3 + sizeof(BulletStyle) + 1] == '}')
	{
		cur_loc++;
		if (temp->next == NULL)
		{
			temp->next = new bullet();
			temp = temp->next;
			memcpy(&(temp->locationX), &ch[cur_loc], sizeof(int) * 3 + sizeof(BulletStyle));
			cur_loc += sizeof(int) * 3 + sizeof(BulletStyle) + 1;
			temp->Set_Parameter_byStyle(temp->bullet_style);
		}
	}
	Tank_info[id]->bullet_head = newhead->next;

	//to_destroyed_bulletinfo.push(optank->bullet_head);
	//cv.notify_one();
}

void Game::hited(char buf[])
{
	int hited_id = -1;
	int cur_loc = 6;
	if (buf[cur_loc] == '(' && buf[cur_loc + sizeof(int) + 1] == ')')
	{
		cur_loc++;
		memcpy(&hited_id, &buf[cur_loc], sizeof(int));
	}
	if (hited_id != -1)
		Tank_info[hited_id]->health -= 21;
}

void Game::myhited()
{
	ptank1->health -= 21;
}


void Game::destoryed(char buf[])
{
	int hited_id = -1;
	int cur_loc = 10;
	if (buf[cur_loc] == '(' && buf[cur_loc + sizeof(int) + 1] == ')')
	{
		cur_loc++;
		memcpy(&hited_id, &buf[cur_loc], sizeof(int));
	}
	Tank_info[hited_id]->isalive = false;
}

void Game::mydestoryed()
{
	ptank1->isalive = false;
}


void set_My_id(int id)
{
	::my_tank_location = id;
}
