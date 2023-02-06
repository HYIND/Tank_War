#include "Tank.h"
using namespace std;


extern bool isonline_game;

extern SOCKET tcp_socket;

queue <bullet*> to_destroyed_bulletinfo;


Tank::Tank(int width, int height)
	:width(width), height(height) {}

Tank::Tank(double X, double Y, int width, int height, double rotate, int speed, bool alive)
	: locationX(X), locationY(Y), width(width), height(height), rotate(rotate), speed(speed), isalive(alive) {}

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
	pRenderTarget->DrawRectangle(D2D1::Rect(float(locationX - HP_halfwidth - 2), HP_locY - HP_halfheight - 2, float(locationX + HP_halfwidth + 2), HP_locY + HP_halfheight + 2), pRed_Brush);
	//画血条
	pRenderTarget->FillRectangle(D2D1::Rect(float(locationX - HP_halfwidth), HP_locY - HP_halfheight, float(locationX - HP_halfwidth + HP_len), HP_locY + HP_halfheight), pRed_Brush);
}

void Tank::DrawTank(ID2D1HwndRenderTarget* pRenderTarget)
{
	int Reloc1 = this->locationX - (this->width) / 2;
	int Reloc2 = this->locationY - (this->height) / 2;

	D2D1_POINT_2F center = D2D1::Point2F(this->locationX, this->locationY);

	DrawTankHP(pRenderTarget);
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(this->rotate + 180, center));
	pRenderTarget->DrawBitmap(Tank_Style_info[tank_style]->Bitmap, D2D1::RectF(Reloc1, Reloc2, Reloc1 + width, Reloc2 + height));
	//pRenderTarget->DrawRectangle(D2D1::RectF(Reloc1, Reloc2, Reloc1 + width, Reloc2 + height), pRed_Brush);
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	//double sin_width1 = sin(rotate * M_PI / 180) * width / 2;
	//double cos_width1 = cos(rotate * M_PI / 180) * width / 2;
	//double cos_height1 = cos(rotate * M_PI / 180) * height / 2;
	//double sin_height1 = sin(rotate * M_PI / 180) * height / 2;

	//pRenderTarget->DrawLine(D2D1::Point2F(locationX - cos_height1, locationY - sin_height1), D2D1::Point2F(locationX + cos_height1, locationY + sin_height1), pRed_Brush);
	//pRenderTarget->DrawLine(D2D1::Point2F(locationX - sin_width1, locationY + cos_width1), D2D1::Point2F(locationX + sin_width1, locationY - cos_width1), pRed_Brush);

};

void bullet::Set_Parameter_byStyle(BulletStyle bulletstyle)
{
	this->width = Bullet_Style_info[bulletstyle]->width;
	this->height = Bullet_Style_info[bulletstyle]->height;
	this->speed = Bullet_Style_info[bulletstyle]->speed;
}

void Tank::Addbullet(BulletStyle bulletstyle) {
	if (this->bullet_head == NULL)
	{
		this->bullet_head = new bullet(this->locationX, this->locationY, this->rotate, bulletstyle, this);
	}
	else {
		bullet* temp = this->bullet_head;
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = new bullet(this->locationX, this->locationY, this->rotate, bulletstyle, this, temp);
	}
}

void bullet::Drawbullet()
{
	D2D1_POINT_2F center = D2D1::Point2F(this->locationX, this->locationY);

	ID2D1Bitmap* pBm = Bullet_Style_info[bullet_style]->Bitmap;
	if (pBm)
	{
		int Reloc1 = this->locationX - (this->width) / 2;
		int Reloc2 = this->locationY - (this->height) / 2;
		int Reloc3 = this->locationX + (this->width) / 2;
		int Reloc4 = this->locationY + (this->height) / 2;

		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(this->rotate, center));
		pRenderTarget->DrawBitmap(pBm, D2D1::RectF(Reloc1, Reloc2, Reloc3, Reloc4));
		//pRenderTarget->DrawRectangle(D2D1::RectF(Reloc1, Reloc2, Reloc3, Reloc4), pRed_Brush);
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	else
	{
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(this->rotate, center));
		pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(this->locationX, this->locationY), this->width, this->height), pRed_Brush);
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	}
	if (this->next != NULL)
		(*(this->next)).Drawbullet();
}

bool bullet::destroy() {
	if (isdestroy)return false;
	isdestroy = true;
	if (this->last != NULL) {
		this->last->next = this->next;
	}
	else
		this->owner->bullet_head = this->next;

	if (this->next != NULL)
		this->next->last = this->last;
	owner->bullet_count--;
	return isdestroy;
}

//mutex mtx;
//condition_variable cv;
//void destory_bulletinfo()
//{
//	bullet* cur = NULL;
//	bullet* temp = NULL;
//	while (TRUE) {
//		std::unique_lock<std::mutex> lck(mtx);
//		cv.wait(lck);
//		while (!to_destroyed_bulletinfo.empty())
//		{
//			cur = to_destroyed_bulletinfo.front();
//			temp = NULL;
//			while (cur)
//			{
//				cur->next = temp;
//				delete(cur);
//				cur = temp;
//			}
//			to_destroyed_bulletinfo.pop();
//		}
//	}
//}