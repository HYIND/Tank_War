#include "Game_Component.h"

map<component_type, Com_Style_info*> com_info;

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

void Init_ComponentMap()
{
	com_info[component_type::BRICK] = new Com_Style_info(BRICK_WIDTH, BRICK_HEIGHT, 60, ResFactory->GetBitMapRes(ResName::brickWall));
	com_info[component_type::IRON] = new Com_Style_info(IRON_WIDTH, IRON_HEIGHT, 9999, ResFactory->GetBitMapRes(ResName::ironWall));
}