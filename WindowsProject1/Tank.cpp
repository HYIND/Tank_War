#include "Tank.h"
using namespace std;

extern ID2D1SolidColorBrush* bullet_pBrush;

extern HDC hdcmem, hdc;
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
	pRenderTarget->DrawBitmap(Tank_Bitmap, D2D1::RectF(Reloc1, Reloc2, Reloc1 + width, Reloc2 + height));
};

void Tank::Tank1_Move(RECT& rect)
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		//б���ƶ�
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
				&& new_locationX < it->cur->locationX + half_width && new_locationX >it->cur->locationX - half_height)
			{
				it->cur->isalive = false;
				/*tank_list.erase(it);*/
				return true;
			}
		}
	}
	return false;
}