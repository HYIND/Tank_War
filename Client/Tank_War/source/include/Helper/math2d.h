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
	Vec2 operator*(float e) const {
		return Vec2(this->x * e, this->y * e);
	}
	Vec2 operator-(const Vec2& other) const {
		return Vec2(this->x - other.x, this->y - other.y);
	}
	Vec2 operator+(const Vec2& other) const {
		return Vec2(this->x + other.x, this->y + other.y);
	}
	float norm() const {
		return sqrt(x * x + y * y);
	}
	Vec2 normalize() const {
		float dis = norm();
		return Vec2(x / dis, y / dis);
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
	Pos2 operator*(float e) const {
		return Pos2(this->x * e, this->y * e);
	}
	Pos2 operator-(const Pos2& other) const {
		return Pos2(this->x - other.x, this->y - other.y);
	}
	Pos2 operator+(const Pos2& other) const {
		return Pos2(this->x + other.x, this->y + other.y);
	}
	Vec2 toVec2() const {
		return Vec2(this->x, this->y);
	}
};

inline float PosDistance(const Pos2& pos1, const Pos2& pos2)
{
	return sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));
}

struct Line2
{

	Pos2 pos[2];
	Line2() {};
	Line2& operator=(const Line2& other)
	{
		if(this == &other)
			return *this;

		pos[0] = other.pos[0];
		pos[1] = other.pos[1];
		return *this;
	}
	Line2(Pos2 pos1, Pos2 pos2)
	{
		pos[0] = pos1;
		pos[1] = pos2;
	};
	Pos2 pos1() { return pos[0]; }
	Pos2 pos2() { return pos[1]; }
};
