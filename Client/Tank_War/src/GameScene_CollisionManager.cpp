#include "GameScene_CollisionManager.h"

extern RECT _rect;

GameSceneManager* GameSceneManager::Instance()
{
	static GameSceneManager* m_Instance = new GameSceneManager();
	return m_Instance;
}

void GameSceneManager::Build_Collision_Tree(Map& map)
{
	if (root)
	{
		Clear_QuadTree(root);
		root = nullptr;
	}
	Load_Map(map);
	Build_QuadTree(root);
}

void GameSceneManager::Quary_Collision(RectObject* object, set<Game_Component*>& result, bool draw)
{
	Quary_Area(object, root, result, draw);
	if (draw)
	{
		for (auto com : result)
		{
			RectObject rect = com->toRECT();
			rect.Draw(false);
		}
	}
}
void GameSceneManager::Quary_Collision(RoundObject* object, set<Game_Component*>& result, bool draw)
{
	Quary_Area(object, root, result, draw);
	if (draw)
	{
		for (auto com : result)
		{
			RectObject rect = com->toRECT();
			rect.Draw(false);
		}
	}
}

void GameSceneManager::Draw_Collision_Tree()
{
	Draw_QuadTree(root);
}

void GameSceneManager::remove(Game_Component* com)
{
	remove_com(root, com);
}

void GameSceneManager::remove_com(QuadNode* node, Game_Component* com)
{
	node->remove(com);
	if (node->issplit)
	{
		for (auto child : node->children)
			remove_com(child, com);
	}
}

void GameSceneManager::Build_QuadTree(QuadNode* node)
{
	if (node->vec.size() <= 5)
		return;

	node->Split();
	RECT rect = node->rectobject.toRECT();
	LOGINFO("Split : {},{},{},{} size: {}", rect.left, rect.top, rect.right, rect.bottom, node->vec.size());
	for (auto child : node->children)
		Build_QuadTree(child);
}

void GameSceneManager::Clear_QuadTree(QuadNode* node)
{
	if (node->issplit)
	{
		for (auto& child : node->children)
		{
			if (child)
				Clear_QuadTree(child);
		}
	}
	delete node;
}

void GameSceneManager::Load_Map(Map& map)
{
	root = new QuadNode(_rect);
	for (auto n : map.Component_info)
		root->vec.emplace_back(n);
}

void GameSceneManager::Draw_QuadTree(QuadNode* node)
{
	node->Draw();
	if (node->issplit)
	{
		for (auto& child : node->children)
		{
			if (child)
				Draw_QuadTree(child);
		}
	}
}

void GameSceneManager::Quary_Area(RectObject* object, QuadNode* node, set<Game_Component*>& result, bool draw)
{
	if (object->collision(node->rectobject))
	{
		if (node->issplit)
		{
			for (auto child : node->children)
			{
				Quary_Area(object, child, result, draw);
			}
		}
		else {
			for (auto com : node->vec)
			{
				result.insert(com);
			}
			if (draw)
				node->Draw(true);
		}
	}
}
void GameSceneManager::Quary_Area(RoundObject* object, QuadNode* node, set<Game_Component*>& result, bool draw)
{
	if (object->collision(node->rectobject))
	{
		if (node->issplit)
		{
			for (auto child : node->children)
			{
				Quary_Area(object, child, result, draw);
			}
		}
		else {
			for (auto com : node->vec)
			{
				result.insert(com);
			}
			if (draw)
				node->Draw(true);
		}
	}
}