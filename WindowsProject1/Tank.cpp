#include "Tank.h"
#include <mutex> 
using namespace std;

extern ID2D1SolidColorBrush* bullet_pBrush;

extern bool isonline_game;

extern SOCKET mysocket;

extern Tank* mytank;
extern Tank* optank;

queue <bullet*> to_destroyed_bulletinfo;

vector <tank_info*> tank_list;
void Get_Initinfo() {
	for (auto it : tank_list)
		it->Get_Initinfo();
}

void Return_Tankinfo() {
	for (auto it : tank_list)
		it->Returnto_Initinfo();
}

void register_tank(Tank* current)
{
	tank_info* t = new tank_info(current);
	tank_list.push_back(t);
}

void Init_all() {
	Return_Tankinfo();
}


Tank::Tank() {}

Tank::Tank(Tank* t) {
	this->width = t->width;
	this->height = t->height;
	this->locationX = t->locationX;
	this->locationY = t->locationY;
	this->direction = t->direction;
	this->isalive = t->isalive;
	this->isregister = t->isregister;
}

Tank::Tank(int width, int height)
{
	this->width = width;
	this->height = width;
	if (!isregister)
	{
		register_tank(this);
		isregister = true;
	}
}

Tank::Tank(int X, int Y, int width, int height, int direction)
{
	this->width = width;
	this->height = height;
	this->locationX = X;
	this->locationY = Y;
	this->direction = direction;
	if (!isregister)
	{
		register_tank(this);
		isregister = true;
	}
}

Tank::Tank(int X, int Y, int width, int height, int direction, bool alive, bool isregister)
{
	this->width = width;
	this->height = height;
	this->locationX = X;
	this->locationY = Y;
	this->direction = direction;
	this->isalive = alive;
	this->isregister = isregister;
}

void Tank::InitTank(int X, int Y, int width, int height, int direction)
{
	this->width = width;
	this->height = height;
	this->locationX = X;
	this->locationY = Y;
	this->direction = direction;
	if (!isregister)
	{
		register_tank(this);
		isregister = true;
	}
}

void Tank::InitTank(int X, int Y, int direction)
{
	this->locationX = X;
	this->locationY = Y;
	this->direction = direction;
	if (!isregister)
	{
		register_tank(this);
		isregister = true;
	}
}

void Tank::InitTank(int width, int height)
{
	this->width = width;
	this->height = height;
	if (!isregister)
	{
		register_tank(this);
		isregister = true;
	}
}

void Tank::DrawTank(ID2D1HwndRenderTarget* pRenderTarget, ID2D1Bitmap* Tank_Bitmap)
{
	int Reloc1 = this->locationX - (this->width) / 2;
	int Reloc2 = this->locationY - (this->height) / 2;

	D2D1_POINT_2F center = D2D1::Point2F(this->locationX, this->locationY);

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(this->direction * 90.0f, center));
	pRenderTarget->DrawBitmap(Tank_Bitmap, D2D1::RectF(Reloc1, Reloc2, Reloc1 + width, Reloc2 + height));
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
};

void Tank::Tank1_Move(RECT& rect)
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		//斜向移动
		//if (GetAsyncKeyState('W') & 0x8000 && GetAsyncKeyState('D') & 0x8000)
		//{
		//	if ((this->locationY - this->height / 2 - 10) < rect.top)
		//		this->locationY = rect.top + this->height / 2;
		//	else this->locationY -= 10;

		//	if ((this->locationX + this->width / 2 + 10) > rect.right)
		//		this->locationX = rect.right - this->width / 2;
		//	else this->locationX += 10;
		//	return;
		//}
		if (this->direction != UP)
			this->direction = UP;
		else if ((this->locationY - this->height / 2 - 10) < rect.top)
			this->locationY = rect.top + this->height / 2;
		else this->locationY -= 10;
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		if (this->direction != DOWN)
			this->direction = DOWN;
		else if ((this->locationY + this->height / 2 + 10) > rect.bottom - 3)
			this->locationY = rect.bottom - this->height / 2 - 3;
		else this->locationY += 10;
	}
	else if (GetAsyncKeyState('A') & 0x8000)
	{
		if (this->direction != LEFT)
			this->direction = LEFT;
		else if ((this->locationX - this->width / 2 - 10) < rect.left)
			this->locationX = rect.left + this->width / 2;
		else this->locationX -= 10;
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		if (this->direction != RIGHT)
			this->direction = RIGHT;
		else if ((this->locationX + this->width / 2 + 10) > rect.right)
			this->locationX = rect.right - this->width / 2;
		else this->locationX += 10;
	}
}

void Tank::Tank2_Move(RECT& rect)
{
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (this->direction != UP)
			this->direction = UP;
		else if ((this->locationY - this->height / 2 - 10) < rect.top)
			this->locationY = rect.top + this->height / 2;
		else this->locationY -= 10;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (this->direction != DOWN)
			this->direction = DOWN;
		else if ((this->locationY + this->height / 2 + 10) > rect.bottom - 3)
			this->locationY = rect.bottom - this->height / 2 - 3;
		else this->locationY += 10;
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		if (this->direction != LEFT)
			this->direction = LEFT;
		else if ((this->locationX - this->width / 2 - 10) < rect.left)
			this->locationX = rect.left + this->width / 2;
		else this->locationX -= 10;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		if (this->direction != RIGHT)
			this->direction = RIGHT;
		else if ((this->locationX + this->width / 2 + 10) > rect.right)
			this->locationX = rect.right - this->width / 2;
		else this->locationX += 10;
	}
}

void Tank::Addbullet() {
	if (this->bullet_head == NULL)
	{
		this->bullet_head = new bullet(this->locationX, this->locationY, this->direction, 20, this);
	}
	else {
		bullet* temp = this->bullet_head;
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = new bullet(this->locationX, this->locationY, this->direction, 20, this, temp);
	}
}

void bullet::Drawbullet(ID2D1HwndRenderTarget* pRenderTarget, ID2D1Bitmap* Bullet_Bitmap)
{
	if (Bullet_Bitmap)
	{
		int Reloc1 = this->locationX - (this->width) / 2;
		int Reloc2 = this->locationY - (this->height) / 2;
		int Reloc3 = this->locationX + (this->width) / 2;
		int Reloc4 = this->locationY + (this->height) / 2;
		pRenderTarget->DrawBitmap(Bullet_Bitmap, D2D1::RectF(Reloc1, Reloc2, Reloc3, Reloc4));
	}
	else
	{
		//D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(100.0f, 100.0f), 100.0f, 50.0f);
 		pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(this->locationX, this->locationY), this->width, this->height), bullet_pBrush);
	}
	if (this->next != NULL)
		(*(this->next)).Drawbullet(pRenderTarget, Bullet_Bitmap);
}

void bullet::Move(RECT rect)
{
	switch (this->direction)
	{
	case UP:
		if ((this->locationY - this->height / 2 - speed) < rect.top || crash(rect, UP))
			destroy();
		else this->locationY -= speed;
		break;
	case DOWN:
		if ((this->locationY + this->height / 2 + speed) > rect.bottom || crash(rect, DOWN))
			destroy();
		else this->locationY += speed;
		break;
	case LEFT:
		if ((this->locationX - this->width / 2 - speed) < rect.left || crash(rect, LEFT))
			destroy();
		else this->locationX -= speed;
		break;
	case RIGHT:
		if ((this->locationX + this->width / 2 + speed) > rect.right || crash(rect, RIGHT))
			destroy();
		else this->locationX += speed;
		break;
	}
	if (this->next != NULL)
		(*(this->next)).Move(rect);
}

void bullet::destroy() {
	if (this->last != NULL) {
		this->last->next = this->next;
	}
	else
		this->owner->bullet_head = this->next;
	if (this->next != NULL)
		this->next->last = this->last;
	else (this->owner)->bullet_head = NULL;
}

bool bullet::crash(RECT rect, int direction)
{
	int new_locationX = this->locationX;
	int new_locationY = this->locationY;
	switch (direction)
	{
	case UP:
		new_locationY = this->locationY - speed;
		break;
	case DOWN:
		new_locationY = this->locationY + speed;
		break;
	case LEFT:
		new_locationX = this->locationX - speed;
		break;
	case RIGHT:
		new_locationX = this->locationX + speed;
		break;
	}
	for (auto& it : tank_list)
	{
		if ((it->cur != this->owner) && it->cur->isalive)
		{
			int half_height = it->cur->height / 2;
			int half_width = it->cur->width / 2;
			if (new_locationY <  it->cur->locationY + half_height && new_locationY >it->cur->locationY - half_height
				&& new_locationX < it->cur->locationX + half_width && new_locationX >it->cur->locationX - half_width)
			{
				if (!isonline_game)
					it->cur->isalive = false;
				else
				{
					this->locationX = new_locationX;
					this->locationY = new_locationY;
					send_destroy(this);
				}
				/*tank_list.erase(it);*/
				return true;
			}
		}
	}
	return false;
}

void send_location(Tank* tank)
{
	char buffer[1024] = "mylocation:";
	int i = sizeof(Tank);
	memcpy(&buffer[11], (char*)tank, sizeof(Tank));
	send(mysocket, buffer, 1023, 0);
}

void send_bullet(bullet* cur)
{
	string str = "mybullet:";
	while (cur != NULL)
	{
		str = str + "{"
			+ to_string(cur->locationX)
			+ ","
			+ to_string(cur->locationY)
			+ "}";
		cur = cur->next;
	}
	send(mysocket, (const char*)&str[0], 1023, 0);
}

void Refresh_opTank(char buf[])
{
	try
	{
		memcpy(optank, &buf[11], 24);
	}
	catch (exception& e)
	{
		return;
	}
}

mutex mtx; // 主线程唤醒子线程的锁
condition_variable cv;//主线程唤醒子线程的条件变量 
void Refresh_opbullet(string& re)
{
	bullet* newhead = new bullet();
	bullet* temp = newhead;

	regex user_reg("[0-9]+");
	sregex_iterator end;
	for (sregex_iterator iter(re.begin(), re.end(), user_reg); iter != end; iter++) {
		string s1 = ((*iter)[0]);
		if (++iter != end)
		{
			string s2 = ((*iter)[0]);
			if (temp->next == NULL)
			{
				temp->next = new bullet();
				temp = temp->next;
				temp->locationX = atoi(s1.c_str());
				temp->locationY = atoi(s2.c_str());
				temp->speed = 20;
				temp->owner = optank;
			}
		}
		else break;
	}
	optank->bullet_head = newhead->next;
	to_destroyed_bulletinfo.push(optank->bullet_head);
	//cv.notify_one();
}

void destory_bulletinfo()
{
	bullet* cur = NULL;
	bullet* temp = NULL;
	while (1) {
		std::unique_lock<std::mutex> lck(mtx);
		cv.wait(lck);
		while (!to_destroyed_bulletinfo.empty())
		{
			cur = to_destroyed_bulletinfo.front();
			temp = NULL;
			while (cur)
			{
				cur->next = temp;
				delete(cur);
				cur = temp;
			}
			to_destroyed_bulletinfo.pop();
		}
		lck.unlock();
	}
}

void send_destroy(bullet* bullet)
{
	string str = "destroy:{";
	str += to_string(bullet->locationX);
	str += ",";
	str += to_string(bullet->locationY);
	str += "}";
	send(mysocket, (const char*)&(str[0]), 1023, 0);
}

void my_destroy()
{
	mytank->isalive = false;
}

void op_destory()
{
	optank->isalive = false;
}