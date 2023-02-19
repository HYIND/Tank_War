#include "Game_Component.h"
#include "D2D.h"

ID2D1Bitmap* brick_wall_pBitmap;
ID2D1Bitmap* iron_wall_pBitmap;

extern HINSTANCE hInst;

void Game_Component::Draw()
{
	int Reloc1 = this->location.x - (this->width) / 2;
	int Reloc2 = this->location.y - (this->height) / 2;
	pRenderTarget->DrawBitmap(Bitmap, D2D1::RectF(Reloc1, Reloc2, Reloc1 + width, Reloc2 + height));
}

int Game_Component::get_health()
{
	try {
		if (type >= component_type::PROP_DEFAULT)
			throw type;
		return health;
	}
	catch (component_type t) {
		LOGERROR("Game_Component::get_health : cannot execute cause component_type is prop {} , not exist health", int(t));
	}
};
void Game_Component::set_health(int health)
{
	try {
		if (type >= component_type::PROP_DEFAULT)
			throw type;
		this->health = health;
	}
	catch (component_type t) {
		LOGERROR("Game_Component::set_health : cannot execute cause component_type is prop {} , cannot set health", int(t));
	}
}
int Game_Component::add_health(int add)
{
	try {
		if (type >= component_type::PROP_DEFAULT)
			throw type;

		health += add;
		return health;
	}
	catch (component_type t) {
		LOGERROR("Game_Component::add_health : cannot execute cause component_type is prop {} , not exist health", int(t));
	}
}
int Game_Component::reduce_health(int reduce)
{
	try {
		if (type >= component_type::PROP_DEFAULT)
			throw type;
		health -= reduce;
		return health;
	}
	catch (component_type t) {
		LOGERROR("Game_Component::reduce_health : cannot execute cause component_type is prop {} , not exist health", int(t));
	}
}

void Init_Component_Resource()
{
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(BRICK_WALL), &brick_wall_pBitmap);
	LoadResourceBitmap(hInst, pIWICFactory, pRenderTarget, L"PNG", MAKEINTRESOURCE(IRON_WALL), &iron_wall_pBitmap);
}