#include "Tank.h"
#include "Game.h"
using namespace std;


extern bool isonline_game;

queue <bullet*> to_destroyed_bulletinfo;


Tank::Tank(int width, int height)
	:RectObject(width, height) {}

Tank::Tank(double x, double y, int width, int height, double rotate, int speed, bool alive)
	: RectObject(x, y, width, height, rotate), speed(speed), isalive(alive) {}

void Tank::Set_Parameter_byStyle(TankStyle tankstyle)
{
	this->width = Tank_Style_info[tankstyle]->width;
	this->height = Tank_Style_info[tankstyle]->height;
	this->speed = Tank_Style_info[tankstyle]->speed;
}

void Tank::DrawTankHP(ID2D1HwndRenderTarget* pRenderTarget)
{

	int HP_locY = this->location.y - height / 2 - 10;
	float HP_halfwidth = 23;
	float HP_halfheight = 1.5;

	//计算血条长度
	float HP_len = (health / 100.f) * HP_halfwidth * 2;

	//画血条框
	pRenderTarget->DrawRectangle(D2D1::Rect(float(location.x - HP_halfwidth - 2), HP_locY - HP_halfheight - 2, float(location.x + HP_halfwidth + 2), HP_locY + HP_halfheight + 2), Brush::pRed_Brush);
	//画血条
	pRenderTarget->FillRectangle(D2D1::Rect(float(location.x - HP_halfwidth), HP_locY - HP_halfheight, float(location.x - HP_halfwidth + HP_len), HP_locY + HP_halfheight), Brush::pRed_Brush);
}

void Tank::DrawTank(ID2D1HwndRenderTarget* pRenderTarget)
{
	int Reloc1 = this->location.x - (this->width) / 2;
	int Reloc2 = this->location.y - (this->height) / 2;

	D2D1_POINT_2F center = D2D1::Point2F(this->location.x, this->location.y);

	DrawTankHP(pRenderTarget);
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(this->rotate + 180, center));
	pRenderTarget->DrawBitmap(Tank_Style_info[tank_style]->Bitmap, D2D1::RectF(Reloc1, Reloc2, Reloc1 + width, Reloc2 + height));
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
};

void Tank::Addbullet(BulletStyle bulletstyle) {
	if (this->bullet_head == NULL)
	{
		this->bullet_head = new bullet(this->location.x, this->location.y, this->rotate, bulletstyle, this);
	}
	else {
		bullet* temp = this->bullet_head;
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = new bullet(this->location.x, this->location.y, this->rotate, bulletstyle, this, temp);
	}
}

void Tank::destory() {
	isalive = false;
	Game::Instance()->AddGIF(location.x - 50, location.y - 50, location.x + 50, location.y + 50, ResFactory->GetGIFRes(ResName::explosionGIF), 1);
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
