#pragma once
#include "Game_Component.h"
#include "Object.h"
#include "Scene.h"
#include "Map.h"

class GameSceneManager
{
	//四叉树节点
	struct QuadNode
	{
		QuadNode* children[4];			//子节点
		bool issplit = false;			//是否已分裂的flag
		vector<Game_Component*> vec;	//存储碰撞体
		RectObject rectobject;

		QuadNode(m_RECT rect) :rectobject(rect)
		{
			for (auto& n : children)
				n = nullptr;
		}

		void Split()		//分裂
		{
			m_RECT rect = rectobject.to_mRECT();
			int x_mid = (rect.left + rect.right) / 2;
			int y_mid = (rect.bottom + rect.top) / 2;
			children[0] = new QuadNode(m_RECT(rect.left, x_mid, rect.top, y_mid));
			children[1] = new QuadNode(m_RECT(x_mid, rect.right, rect.top, y_mid));
			children[2] = new QuadNode(m_RECT(rect.left, x_mid, y_mid, rect.bottom));
			children[3] = new QuadNode(m_RECT(x_mid, rect.right, y_mid, rect.bottom));
			for (auto com : vec)
			{
				for (auto child : children)
				{
					if (com->collision(child->rectobject))
					{
						child->vec.push_back(com);
					}
				}
			}
			this->issplit = true;
		}

		void Draw(bool flag = false)
		{
			RECT rect = rectobject.toRECT();
			if (!flag)
				pRenderTarget->DrawRectangle(D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom), Brush::pRed_Brush);
			else
				pRenderTarget->DrawRectangle(D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom), Brush::pGreen_Brush);
		}

		void remove(Game_Component* com)
		{
			for (auto it = vec.begin(); it != vec.end(); it++)
			{
				if (*it == com)
				{
					vec.erase(it);
					break;
				}
			}
		}
	};

public:
	static GameSceneManager* Instance();
	void Build_Collision_Tree(Map& map);														//构建碰撞检测四叉树
	void Quary_Collision(RectObject* object, set<Game_Component*>& result, bool draw = false);	//查询物体碰撞结果
	void Quary_Collision(RoundObject* object, set<Game_Component*>& result, bool draw = false);	//查询物体碰撞结果
	void Draw_Collision_Tree();																	//测试用，画四叉树分区图
	void remove(Game_Component* com);

private:
	GameSceneManager() {};
	void Build_QuadTree(QuadNode* node);
	void Clear_QuadTree(QuadNode* node);
	void Load_Map(Map& map);
	void Draw_QuadTree(QuadNode* node);
	void Quary_Area(RectObject* object, QuadNode* node, set<Game_Component*>& result, bool draw);
	void Quary_Area(RoundObject* object, QuadNode* node, set<Game_Component*>& result, bool draw);
	void remove_com(QuadNode* node, Game_Component* com);

private:
	QuadNode* root;		//四叉树根节点
};