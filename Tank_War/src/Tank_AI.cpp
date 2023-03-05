#include "Tank_AI.h"

#define _USE_MATH_DEFINES
#include <math.h>

AI_control* AI = NULL;
void AI_Init() {

	AI = new AI_control();
	AI->AI_Tank = Game::Instance()->ptank2;
}

void AI_control::AI_Move()
{
	if (AI_Tank->get_rotate() != goal_rotate) {
		if (AI_Tank->get_rotate() < goal_rotate) AI_Rotate(true);
		else AI_Rotate(false);
	}
	if (abs(goal_rotate - AI_Tank->get_rotate()) < 40)
		Game::Instance()->Tank_Move(AI_Tank, true);
	if (abs(goal_rotate - AI_Tank->get_rotate()) < 30)
	{
		Game::Instance()->Tank_shot(AI_Tank);
	}
}

void AI_control::AI_Rotate(bool forward) {
	Game::Instance()->AI_Rotate(this, forward);
}

void AI_control::Get_goal() {
	goal_x = Game::Instance()->ptank1->get_locationX();
	goal_y = Game::Instance()->ptank1->get_locationY();
	double detalX = goal_x - AI_Tank->get_locationX();
	double detalY = goal_y - AI_Tank->get_locationY();
	goal_rotate = atan2(detalX, -detalY) * 180 / M_PI;

}

void AI_calculate() {
	if (!Game::Instance()->ptank1->isalive) return;
	AI->Get_goal();
}
void AI_Track() {
	if (!Game::Instance()->ptank1->isalive) return;
	AI->AI_Track();
}

void AI_control::AI_Track() {
	this->AI_Move();
}
