#pragma once

bool collision(
	double x1,double y1, int width1, int height1,
	double x2,double y2, int width2, int height2
);

bool collision_obb(
	double x1, double y1, int width1, int height1, double rotate1,
	double x2, double y2, int width2, int height2, double rotate2 = 90
);