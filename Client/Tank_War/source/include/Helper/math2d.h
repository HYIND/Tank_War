#pragma once

#include <cmath>

struct Pos2;

//二维向量
struct Vec2
{
	float x;
	float y;
	Vec2() :x(0), y(0) {}
	Vec2(float x, float y) :x(x), y(y) {}
	Vec2 operator*(float e) {
		return Vec2(this->x * e, this->y * e);
	}
	Vec2 operator-(Vec2 other) {
		return Vec2(this->x - other.x, this->y - other.y);
	}
	Vec2 operator+(Vec2 other) {
		return Vec2(this->x + other.x, this->y + other.y);
	}
	float norm() {
		return sqrt(x * x + y * y);
	}
};

struct Pos2
{
	float x;
	float y;
	Pos2() :x(0), y(0) {};
	Pos2(float x, float y) :x(x), y(y) {}
	Pos2(Vec2 vec) {
		this->x = vec.x;
		this->y = vec.y;
	};
	Pos2 operator*(float e) {
		return Pos2(this->x * e, this->y * e);
	}
	Pos2 operator-(Pos2 other) {
		return Pos2(this->x - other.x, this->y - other.y);
	}
	Pos2 operator+(Pos2 other) {
		return Pos2(this->x + other.x, this->y + other.y);
	}
	Vec2 toVec2() {
		return Vec2(this->x, this->y);
	}
};

struct Line2
{

	Pos2 pos[2];
	Line2() {};
	Line2(Pos2 pos1, Pos2 pos2)
	{
		pos[0] = pos1;
		pos[1] = pos2;
	};
	Pos2 pos1() { return pos[0]; }
	Pos2 pos2() { return pos[1]; }
};
