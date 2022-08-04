#pragma once

bool collision(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2);

#define  Object_Check(A,B) collision(A->locationX, A->locationY,A->width, A->height,B->locationX, B->locationY,B->width, B->height)

#define  Object_Check_predict(A,B) if(collision(new_locationX, new_locationY,A->width, A->height,B->locationX, B->locationY,B->width, B->height))\
		{\
		switch (ptank->direction)\
		{\
		case UP:\
			new_locationY = B->locationY + B->height / 2 + A->height / 2;\
			break;\
		case DOWN:\
			new_locationY = B->locationY - B->height / 2 - A->height / 2;\
			break;\
		case LEFT:\
			new_locationX = B->locationX + B->width / 2 + A->width / 2;\
			break;\
		case RIGHT:\
			new_locationX = B->locationX - B->width / 2 - A->width / 2;\
			break;\
		}\
		A->locationX = new_locationX;\
		A->locationY = new_locationY;\
		return;\
		}