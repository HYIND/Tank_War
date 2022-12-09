#pragma once
#include "header.h"
#include "Game.h"

class AI_control {
public:
	Tank* AI_Tank = NULL;
	double goal_rotate;
	double goal_x;
	double goal_y;
	bool Move_forward;

public:
	bool get_distance();

	void AI_Move();

	void AI_Rotate(bool forward);

	void Get_goal();

	void AI_Track();
};

void AI_Init();
void AI_Track();
void AI_calculate();
