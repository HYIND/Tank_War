#include "Bullet.h"

Style_info* bullet::Get_Parameter_byStyle(BulletStyle bulletstyle)
{
	return Bullet_Style_info[bulletstyle];
}

void bullet::Drawbullet()
{
	D2D1_POINT_2F center = D2D1::Point2F(this->location.x, this->location.y);

	ID2D1Bitmap* pBm = Bullet_Style_info[bullet_style]->Bitmap;
	if (pBm)
	{
		double Reloc1 = this->location.x - (this->radius) / 2;
		double Reloc2 = this->location.y - (this->radius) / 2;
		double Reloc3 = this->location.x + (this->radius) / 2;
		double Reloc4 = this->location.y + (this->radius) / 2;

		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(this->rotate, center));
		pRenderTarget->DrawBitmap(pBm, D2D1::RectF(Reloc1, Reloc2, Reloc3, Reloc4));
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	}
	else
	{
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(this->rotate, center));
		pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(this->location.x, this->location.y), this->radius / 2, this->radius / 2), Brush::pRed_Brush);
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