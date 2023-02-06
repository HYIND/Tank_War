#include "Tank_AI.h"

#define _USE_MATH_DEFINES
#include <math.h>

AI_control* AI = NULL;
void AI_Init() {

	AI = new AI_control();
	AI->AI_Tank = Cur_Game->ptank2;
}

void AI_control::AI_Move()
{
	if (AI_Tank->rotate != goal_rotate) {
		if (AI_Tank->rotate < goal_rotate) AI_Rotate(true);
		else AI_Rotate(false);
	}
	if (abs(goal_rotate - AI_Tank->rotate) < 40)
		Cur_Game->Tank_Move(AI_Tank, true);
	if (abs(goal_rotate - AI_Tank->rotate) < 30)
	{
		Cur_Game->Tank_shot(AI_Tank);
	}
}

void AI_control::AI_Rotate(bool forward) {
	Cur_Game->AI_Rotate(this, forward);
}

void AI_control::Get_goal() {
	goal_x = Cur_Game->ptank1->locationX;
	goal_y = Cur_Game->ptank1->locationY;
	double detalX = goal_x - AI_Tank->locationX;
	double detalY = goal_y - AI_Tank->locationY;
	goal_rotate = atan2(detalX, -detalY) * 180 / M_PI;

}

void AI_calculate() {
	if (!Cur_Game->ptank1->isalive) return;
	AI->Get_goal();
}
void AI_Track() {
	if (!Cur_Game->ptank1->isalive) return;
	AI->AI_Track();
}

void AI_control::AI_Track() {
	this->AI_Move();
}
