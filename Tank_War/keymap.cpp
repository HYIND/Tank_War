#include "keymap.h"

map<enum class keybroad, int> key_map_p1 = {
	{ keybroad::UP,87 },{ keybroad::DOWN,83 },
	{ keybroad::LEFT,65 },{ keybroad::RIGHT,68 },
	{ keybroad::FIRE,VK_SPACE }
};

map<enum class keybroad, int> key_map_p2 = {
	{ keybroad::UP,VK_UP },{ keybroad::DOWN,VK_DOWN },
	{ keybroad::LEFT,VK_LEFT },{ keybroad::RIGHT,VK_RIGHT },
	{ keybroad::FIRE,VK_OEM_2 }
};