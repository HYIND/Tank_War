#include "Tank.h"
using namespace std;


extern bool isonline_game;

extern SOCKET mysocket;

queue <bullet*> to_destroyed_bulletinfo;

//void Get_Initinfo() {
//	for (auto it : tank_list)
//		it->Get_Initinfo();
//}
//
//void Return_Tankinfo() {
//	for (auto it : tank_list)
//		it->Returnto_Initinfo();
//}
//
//void register_tank(Tank* current)
//{
//	tank_info* t = new tank_info(current);
//	tank_list.push_back(t);
//}

//void Init_all() {
//	Return_Tankinfo();
//}


Tank::Tank(int width, int height)
	:width(width), height(height) {}

Tank::Tank(int X, int Y, int width, int height, int direction, int speed, bool alive)
	: locationX(X), locationY(Y), width(width), height(height), direction(direction), speed(speed), isalive(alive) {}

void Tank::Set_Parameter_byStyle(TankStyle tankstyle)
{
	this->width = Tank_Style_info[tankstyle]->width;
	this->height = Tank_Style_info[tankstyle]->height;
	this->speed = Tank_Style_info[tankstyle]->speed;
}

void Tank::DrawTankHP(ID2D1HwndRenderTarget* pRenderTarget)
{

	int HP_locY = this->locationY - height / 2 - 10;
	float HP_halfwidth = 23;
	float HP_halfheight = 1.5;

	//计算血条长度
	float HP_len = (health / 100.f) * HP_halfwidth * 2;

	//画血条框
	pRenderTarget->DrawRectangle(D2D1::Rect(locationX - HP_halfwidth - 2, HP_locY - HP_halfheight - 2, locationX + HP_halfwidth + 2, HP_locY + HP_halfheight + 2), pRed_Brush);
	//画血条
	pRenderTarget->FillRectangle(D2D1::Rect(locationX - HP_halfwidth, HP_locY - HP_halfheight, locationX - HP_halfwidth + HP_len, HP_locY + HP_halfheight), pRed_Brush);
}

void Tank::DrawTank(ID2D1HwndRenderTarget* pRenderTarget)
{
	int Reloc1 = this->locationX - (this->width) / 2;
	int Reloc2 = this->locationY - (this->height) / 2;

	D2D1_POINT_2F center = D2D1::Point2F(this->locationX, this->locationY);

	DrawTankHP(pRenderTarget);
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(this->direction * 90.0f, center));
	pRenderTarget->DrawBitmap(Tank_Style_info[tank_style]->Bitmap, D2D1::RectF(Reloc1, Reloc2, Reloc1 + width, Reloc2 + height));
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
};

//void Tank::Tank_Move(Direction direction)
//{
//	switch (direction)
//	{
//	case UP:
//		this->locationY -= speed;
//		break;
//	case DOWN:
//		if (this->direction != DOWN)
//			this->direction = DOWN;
//		this->locationY += 10;
//		break;
//	case LEFT:
//		if (this->direction != LEFT)
//			this->direction = LEFT;
//		this->locationX -= 10;
//		break;
//	case RIGHT:
//		if (this->direction != RIGHT)
//			this->direction = RIGHT;
//		this->locationX += 10;
//		break;
//	default:
//		break;
//	}
//
//	//	//斜向移动
//	//	if (GetAsyncKeyState('W') & 0x8000 && GetAsyncKeyState('D') & 0x8000)
//	//	{
//	//		if ((this->locationY - this->height / 2 - 10) < rect.top)
//	//			this->locationY = rect.top + this->height / 2;
//	//		else this->locationY -= 10;
//
//	//		if ((this->locationX + this->width / 2 + 10) > rect.right)
//	//			this->locationX = rect.right - this->width / 2;
//	//		else this->locationX += 10;
//	//		return;
//	//	}
//}

//void Tank::Tank2_Move(RECT& rect)
//{
//	if (GetAsyncKeyState(VK_UP) & 0x8000)
//	{
//		if (this->direction != UP)
//			this->direction = UP;
//		else if ((this->locationY - this->height / 2 - 10) < rect.top)
//			this->locationY = rect.top + this->height / 2;
//		else this->locationY -= 10;
//	}
//	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
//	{
//		if (this->direction != DOWN)
//			this->direction = DOWN;
//		else if ((this->locationY + this->height / 2 + 10) > rect.bottom - 3)
//			this->locationY = rect.bottom - this->height / 2 - 3;
//		else this->locationY += 10;
//	}
//	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
//	{
//		if (this->direction != LEFT)
//			this->direction = LEFT;
//		else if ((this->locationX - this->width / 2 - 10) < rect.left)
//			this->locationX = rect.left + this->width / 2;
//		else this->locationX -= 10;
//	}
//	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
//	{
//		if (this->direction != RIGHT)
//			this->direction = RIGHT;
//		else if ((this->locationX + this->width / 2 + 10) > rect.right)
//			this->locationX = rect.right - this->width / 2;
//		else this->locationX += 10;
//	}
//}


void bullet::Set_Parameter_byStyle(BulletStyle bulletstyle)
{
	this->width = Bullet_Style_info[bulletstyle]->width;
	this->height = Bullet_Style_info[bulletstyle]->height;
	this->speed = Bullet_Style_info[bulletstyle]->speed;
}

void Tank::Addbullet(BulletStyle bulletstyle) {
	if (this->bullet_head == NULL)
	{
		this->bullet_head = new bullet(this->locationX, this->locationY, this->direction, bulletstyle, this);
	}
	else {
		bullet* temp = this->bullet_head;
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = new bullet(this->locationX, this->locationY, this->direction, bulletstyle, this, temp);
	}
}

void bullet::Drawbullet()
{
	ID2D1Bitmap* pBm = Bullet_Style_info[bullet_style]->Bitmap;
	if (pBm)
	{
		int Reloc1 = this->locationX - (this->width) / 2;
		int Reloc2 = this->locationY - (this->height) / 2;
		int Reloc3 = this->locationX + (this->width) / 2;
		int Reloc4 = this->locationY + (this->height) / 2;
		pRenderTarget->DrawBitmap(pBm, D2D1::RectF(Reloc1, Reloc2, Reloc3, Reloc4));
	}
	else
	{
		//D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(100.0f, 100.0f), 100.0f, 50.0f);
		pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(this->locationX, this->locationY), this->width, this->height), pRed_Brush);
	}
	if (this->next != NULL)
		(*(this->next)).Drawbullet();
}

void bullet::destroy() {
	if (this->last != NULL) {
		this->last->next = this->next;
	}
	else
		this->owner->bullet_head = this->next;

	if (this->next != NULL)
		this->next->last = this->last;
	owner->bullet_count--;
}

mutex mtx;
condition_variable cv;
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