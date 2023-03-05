#pragma once
#include "Game_Component.h"
#include "Object.h"
#include "Scene.h"
#include "Map.h"

class GameSceneManager
{
	//�Ĳ����ڵ�
	struct QuadNode
	{
		QuadNode* children[4];			//�ӽڵ�
		bool issplit = false;			//�Ƿ��ѷ��ѵ�flag
		vector<Game_Component*> vec;	//�洢��ײ��
		RectObject rectobject;

		QuadNode(m_RECT rect) :rectobject(rect)
		{
			for (auto& n : children)
				n = nullptr;
		}

		void Split()		//����
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
	void Build_Collision_Tree(Map& map);														//������ײ����Ĳ���
	void Quary_Collision(RectObject* object, set<Game_Component*>& result, bool draw = false);	//��ѯ������ײ���
	void Quary_Collision(RoundObject* object, set<Game_Component*>& result, bool draw = false);	//��ѯ������ײ���
	void Draw_Collision_Tree();																	//�����ã����Ĳ�������ͼ
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
	QuadNode* root;		//�Ĳ������ڵ�
};