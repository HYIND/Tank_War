#include "Game_Component.h"

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