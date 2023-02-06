#pragma once

bool collision(
	double x1, double y1, int width1, int height1,
	double x2, double y2, int width2, int height2
);

bool collision_obb(
	double x1, double y1, int width1, int height1, double rotate1,
	double x2, double y2, int width2, int height2, double rotate2 = 90
);

enum Collision_Pos {
	NullPos = 0,
	LeftTopCorner = 1,
	RightTopCorner,
	RightBottomCorner,
	LeftBottomCorner,
	LeftBroader,
	TopBroader,
	RightBroader,
	BottomBroader
};
Collision_Pos collision_round_aabb_withInfo(
	double x1, double y1,
	int radius, double rotate1,
	double x2, double y2,
	int width2, int height2, double rotate2 = 90.0,
	/*output*/ double* distance_out = nullptr
);

bool collision_broader(double x, double y, int width, int height, double rotate = 90);

