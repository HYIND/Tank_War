#pragma once

#include "header.h"

struct Pos;

//��ά����
struct Vec
{
	double x;
	double y;
	Vec() {}
	//Vec(Pos pos) {
	//	this->x = pos.x;
	//	this->y = pos.y;
	//};
	Vec(double x, double y) :x(x), y(y) {}
	Vec operator*(double e) {
		return Vec(this->x * e, this->y * e);
	}
	Vec operator-(Vec other) {
		return Vec(this->x - other.x, this->y - other.y);
	}
	Vec operator+(Vec other) {
		return Vec(this->x + other.x, this->y + other.y);
	}
	double norm() {
		return sqrt(x * x + y * y);
	}
};
double dot(Vec vec1, Vec  vec2);	//�ڻ�
double cross(Vec vec1, Vec vec2);	//���

struct Pos
{
	double x;
	double y;
	Pos() {};
	Pos(Vec vec) {
		this->x = vec.x;
		this->y = vec.y;
	};
	Pos(double x, double y) :x(x), y(y) {}
	Pos operator*(double e) {
		return Pos(this->x * e, this->y * e);
	}
	Pos operator-(Pos other) {
		return Pos(this->x - other.x, this->y - other.y);
	}
	Pos operator+(Pos other) {
		return Pos(this->x + other.x, this->y + other.y);
	}
	Vec toVec() {
		return Vec(this->x, this->y);
	}
	void Draw(bool isFill = false, bool redcolor = true);
};
double distance(Pos pos1, Pos pos2);

struct Line
{

	Pos pos[2];
	Line() {};
	Line(Pos pos1, Pos pos2)
	{
		pos[0] = pos1;
		pos[1] = pos2;
	};
	Pos pos1() { return pos[0]; }
	Pos pos2() { return pos[1]; }
	void Draw(bool redcolor = true);
};
double distance(Pos pos, Line line);

class Object {
protected:
	Pos location;
public:
	Object() {};
	Object(Pos pos) :location(pos) {}
	Object(double x, double y) :location(x, y) {}
	Pos get_location() { return location; }
	double get_locationX() { return location.x; }
	double get_locationY() { return location.y; }
	void set_location(Pos pos) { location = pos; }
	void set_locationX(double x) { location.x = x; }
	void set_locationY(double y) { location.y = y; }

	//virtual bool collision_broader() = 0;
};


struct m_RECT
{
	int left;
	int right;
	int top;
	int bottom;

	m_RECT() {}
	m_RECT(int left, int right, int top, int bottom)
		:left(left), right(right), top(top), bottom(bottom) {}
	m_RECT(RECT& rect)
		:left(rect.left), right(rect.right), top(rect.top), bottom(rect.bottom) {}
	RECT toRECT()
	{
		RECT rect;
		rect.left = left;
		rect.right = right;
		rect.top = top;
		rect.bottom = bottom;
		return rect;
	}
};
class RectObject :public Object
{
protected:
	int width;
	int height;
	double rotate = 90.0;
public:
	RectObject() {};
	RectObject(int width, int height)
		:width(width), height(height) {};
	RectObject(Pos pos, int width, int height)
		:Object(pos), width(width), height(height) {};
	RectObject(double x, double y, int width, int height)
		:Object(x, y), width(width), height(height) {}
	RectObject(m_RECT rect)
		:RectObject(rect.toRECT()) {}
	RectObject(RECT rect)
	{
		rotate = 90.0;
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		location.x = (rect.right + rect.left) / 2;
		location.y = (rect.bottom + rect.top) / 2;
	}
	RectObject(Pos(pos), int width, int height, double rotate)
		:Object(pos), width(width), height(height), rotate(rotate) {}
	RectObject(double x, double y, int width, int height, double rotate)
		:Object(x, y), width(width), height(height), rotate(rotate) {}

	int get_width() { return width; }
	int get_height() { return height; }
	double get_rotate() { return rotate; }
	void set_width(int width) { this->width = width; }
	void set_height(int height) { this->height = height; }
	void set_rotate(double rotate) { this->rotate = rotate; }

	m_RECT to_mRECT();
	RECT toRECT();

	void Draw(bool redcolor);
	virtual void Draw();
	virtual bool collision(RectObject& other);
	virtual bool collision_broader();

private:
	bool collision_aabb(RectObject& other);
	bool collision_obb(RectObject& other);
	void get_lines(Line lines[4]);
};

struct Collision_state
{
	Pos pos;
	double rotate;
	Collision_state() {};
	Collision_state(Pos pos, double rotate) :pos(pos), rotate(rotate) {}
};

class RoundObject :public Object
{
protected:
	double rotate = 90.0;
	double radius;
public:
	RoundObject() {};
	RoundObject(double radius)
		:radius(radius) {};
	RoundObject(Pos pos, double radius)
		:Object(pos), radius(radius) {};
	RoundObject(double x, double y, double radius)
		:Object(x, y), radius(radius) {}
	RoundObject(Pos pos, double radius, double rotate)
		:Object(pos), radius(radius), rotate(rotate) {}
	RoundObject(double x, double y, double radius, double rotate)
		:Object(x, y), radius(radius), rotate(rotate) {}

	double get_radius() { return radius; }
	double get_rotate() { return rotate; }
	void set_radius(double radius) { this->radius = radius; }
	void set_rotate(double rotate) { this->rotate = rotate; }

	void Draw(bool isFill, bool redcolor);
	virtual void Draw();
	virtual bool collision(RectObject& other);
	virtual bool collision(RectObject& other, Pos& ori_RoundPos,
		/*output*/ Collision_state* state_out);
	virtual bool collision_broader();

private:
	bool collision_aabb(RectObject& other);
	bool collision_obb(RectObject& other);
	bool collision_aabb(RectObject& other, Pos& ori_RoundPos,
		/*output*/ Collision_state* state_out);
	bool collision_obb(RectObject& other, Pos& ori_RoundPos,
		/*output*/ Collision_state* state_out);
};

enum Line_Round_Crossresult
{
	NullCrossPos = 0,
	OneCrossPos = 1,
	TwoCrossPos = 2
};
Line_Round_Crossresult cross(Line& line, RoundObject& round, Pos pos[2]);
bool cross(Line line1, Line line2, Pos* result);
