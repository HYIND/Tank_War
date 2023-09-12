#include "Object.h"
#include "Scene.h"

extern RECT _rect;

double dot(Vec vec1, Vec  vec2)		//内积
{
	return vec1.x * vec2.x + vec1.y * vec2.y;
}
double cross(Vec vec1, Vec vec2)	//外积 
{
	return vec1.x * vec2.y - vec1.y * vec2.x;
}

double distance(Pos pos1, Pos pos2)
{
	double detalX = pos1.x - pos2.x;
	double detalY = pos1.y - pos2.y;
	return sqrt(detalX * detalX + detalY * detalY);
}
double distance(Pos pos, Line line)
{
	Vec vec1 = (line.pos1() - line.pos2()).toVec();
	Vec vec2 = (pos - line.pos2()).toVec();
	return abs(cross(vec1, vec2) / vec1.norm());
}

Line_Round_Crossresult cross(Line& line, RoundObject& round, Pos pos[2])
{
	Vec vec((line.pos1() - line.pos2()).toVec());
	double dst = distance(round.get_location(), line);
	if (dst > round.get_radius() / 2)
		return NullCrossPos;
	else if (dst < round.get_radius() / 2)
	{
		double distance_p2_d = fabs(dot(vec, Vec((round.get_location() - line.pos2()).toVec())) / vec.norm());
		double distance_p2_p1 = distance(line.pos1(), line.pos2());
		Pos d = line.pos2() + Pos(vec * (distance_p2_d / distance_p2_p1));
		double distance_d_crosspos = sqrt((round.get_radius() * round.get_radius() / 4.0) - dst * dst);
		pos[0] = d - Pos(vec * (distance_d_crosspos / distance_p2_p1));
		pos[1] = d + Pos(vec * (distance_d_crosspos / distance_p2_p1));
		return TwoCrossPos;
	}
	else
	{
		double distance_p1_d = fabs(dot(vec, Vec((round.get_location() - line.pos2()).toVec())) / vec.norm());
		double distance_p1_p2 = distance(line.pos1(), line.pos2());
		Pos d = line.pos2() + Pos(vec * (distance_p1_d / distance_p1_p2));
		pos[0] = d;
		pos[1] = d;
		return OneCrossPos;
	}
}

bool cross(Line line1, Line line2, Pos* result)
{
	bool col_result = false;
	//检测Y轴碰撞,碰撞则返回true;
	double min_x_line1 = min(line1.pos1().x, line1.pos2().x);
	double max_x_line1 = max(line1.pos1().x, line1.pos2().x);
	double min_y_line1 = min(line1.pos1().y, line1.pos2().y);
	double max_y_line1 = max(line1.pos1().y, line1.pos2().y);
	double min_x_line2 = min(line2.pos1().x, line2.pos2().x);
	double max_x_line2 = max(line2.pos1().x, line2.pos2().x);
	double min_y_line2 = min(line2.pos1().y, line2.pos2().y);
	double max_y_line2 = max(line2.pos1().y, line2.pos2().y);


	//	跨立实验
	//检测Y轴碰撞,碰撞则返回true
	bool collisionY =
		min(line1.pos1().y, line1.pos2().y) <= max(line2.pos1().y, line2.pos2().y) ==
		max(line1.pos1().y, line1.pos2().y) >= min(line2.pos1().y, line2.pos2().y);
	//检测X轴碰撞,碰撞则返回true
	bool collisionX =
		min(line1.pos1().x, line1.pos2().x) <= max(line2.pos1().x, line2.pos2().x) ==
		max(line1.pos1().x, line1.pos2().x) >= min(line2.pos1().x, line2.pos2().x);

	//	向量检查
	Vec vec_line1 = (line1.pos2() - line1.pos1()).toVec();
	Vec	vec1 = (line1.pos1() - line2.pos1()).toVec();
	Vec	vec2 = (line1.pos1() - line2.pos2()).toVec();
	bool vec_result = cross(vec_line1, vec1) >= 0.0 == cross(vec_line1, vec2) <= 0.0;

	col_result = collisionX && collisionY && vec_result;
	if (col_result == false || result == nullptr)return col_result;


	double d1 = abs(cross(vec_line1, (line1.pos1() - line2.pos1()).toVec()));
	double d2 = abs(cross(vec_line1, (line1.pos2() - line2.pos2()).toVec()));
	double t = d1 / (d1 + d2);
	*result = line2.pos1() + (line2.pos2() - line2.pos1()) * t;

	return col_result;
}

m_RECT RectObject::to_mRECT()
{
	try {
		if (rotate != 90.0)
			throw(rotate);
		return m_RECT(location.x - width / 2 + 0.5, location.x + width / 2 + 0.5, location.y - height / 2 + 0.5, location.y + height / 2 + 0.5);
	}
	catch (double e) {
		LOGERROR("RectObject::to_mRECT : cannot execute cause rotate = {} ,not 90.0", rotate);
	}
}
RECT RectObject::toRECT()
{
	try {
		if (rotate != 90.0)
			throw(rotate);
		RECT rect;
		rect.left = location.x - width / 2;
		rect.right = location.x + width / 2;
		rect.top = location.y - height / 2;
		rect.bottom = location.y + height / 2;
		return rect;
	}
	catch (double e) {
		LOGERROR("RectObject::toRECT : cannot execute cause rotate = {} ,not 90.0", rotate);
	}
}

bool RectObject::collision(RectObject& other)
{
	if ((this->rotate == 90 || this->rotate == 270) && other.rotate == 90)
		return collision_aabb(other);
	return collision_obb(other);
}

/*
* 判断OOB包围盒与边界的碰撞
* 输入：OOB包围盒的中心坐标x、y、宽、高、角度
* 输出：碰撞结果（true为发生碰撞，false为未发生碰撞）
*/
bool RectObject::collision_broader()
{
	double sin_width = sin(rotate * M_PI / 180) * width / 2;
	double cos_width = cos(rotate * M_PI / 180) * width / 2;
	double cos_height = cos(rotate * M_PI / 180) * height / 2;
	double sin_height = sin(rotate * M_PI / 180) * height / 2;

	Vec axis[4] = {
		{ location.x + sin_width + cos_height, location.y + cos_width + sin_height },
		{ location.x + sin_width - cos_height, location.y + cos_width - sin_height },
		{ location.x - sin_width + cos_height, location.y - cos_width + sin_height },
		{ location.x - sin_width - cos_height, location.y - cos_width - sin_height }
	};

	for (int i = 0; i < 4; i++)
	{
		if (axis[i].x <= _rect.left || axis[i].x >= _rect.right)
			return true;
		if (axis[i].y <= _rect.top || axis[i].y >= _rect.bottom)
			return true;
	}
	return false;
}

bool RectObject::collision_aabb(RectObject& other)
{
	//检测Y轴碰撞,碰撞则返回true;
	bool collisionY = this->location.y - this->height / 2 < other.location.y + other.height / 2 == this->location.y + this->height / 2 > other.location.y - other.height / 2;
	//检测X轴碰撞,碰撞则返回true
	bool collisionX = this->location.x - this->width / 2 < other.location.x + other.width / 2 == this->location.x + this->width / 2 > other.location.x - other.width / 2;

	if (collisionX && collisionY)
	{
		return true;
	}
	return false;
}

/*
* 判断两OOB包围盒的碰撞
* 输入：两个OOB包围盒的中心坐标x、y、宽、高、角度
* 输出：碰撞结果（true为发生碰撞，false为未发生碰撞）
*/
bool RectObject::collision_obb(RectObject& other)
{
	double sin_mywidth = sin(this->rotate * M_PI / 180) * this->width / 2;
	double cos_mywidth = cos(this->rotate * M_PI / 180) * this->width / 2;
	double cos_myheight = cos(this->rotate * M_PI / 180) * this->height / 2;
	double sin_myheight = sin(this->rotate * M_PI / 180) * this->height / 2;

	double sin_otherwidth = sin(other.rotate * M_PI / 180) * other.width / 2;
	double cos_otherwidth = cos(other.rotate * M_PI / 180) * other.width / 2;
	double cos_otherheight = cos(other.rotate * M_PI / 180) * other.height / 2;
	double sin_otherheight = sin(other.rotate * M_PI / 180) * other.height / 2;

	Vec axis[4] = {
		{ cos_myheight,sin_myheight },	//当前矩形沿着height方向的向量，模为this->height /2,
		{ sin_mywidth, -cos_mywidth },	//当前矩形沿着width方向的向量，模为this->width /2，
		{ cos_otherheight, sin_otherheight },	//另一矩形沿着height方向的向量，模为other.height /2,
		{ sin_otherwidth, -cos_otherwidth }		//另一矩形沿着width方向的向量，模为other.width /2，
	};

	Vec centre = { this->location.x - other.location.x,this->location.y - other.location.y };	//两矩形中心连线方向的向量

	for (int i = 0; i < 4; i++)
	{
		if (fabs(dot(centre, axis[i])) >=
			fabs(dot(axis[0], axis[i])) + fabs(dot(axis[1], axis[i])) + fabs(dot(axis[2], axis[i])) + fabs(dot(axis[3], axis[i])))
			return false;
	}
	return true;
}

bool RoundObject::collision(RectObject& other)
{
	if (other.get_rotate() == 90)
		return collision_aabb(other);
	return collision_obb(other);
}

bool RoundObject::collision(RectObject& other, Pos& ori_RoundPos,/*output*/Collision_state* state_out)
{
	if (other.get_rotate() == 90.0)
		return collision_aabb(other, ori_RoundPos, state_out);
	return collision_obb(other, ori_RoundPos, state_out);
}

bool RoundObject::collision_broader()
{
	static RectObject _rectobject(_rect);

	bool result = false;

	double Rect_left = _rectobject.get_locationX() - _rectobject.get_width() / 2;
	double Rect_right = _rectobject.get_locationX() + _rectobject.get_width() / 2;
	double Rect_top = _rectobject.get_locationY() - _rectobject.get_height() / 2;
	double Rect_bottom = _rectobject.get_locationY() + _rectobject.get_height() / 2;

	Pos Nearest_pos(
		max(Rect_left, min(this->location.x, Rect_right)),
		max(Rect_top, min(this->location.y, Rect_bottom))
	);
	if (distance(Nearest_pos, this->location) < radius / 2 == false)
		return true;

	if (this->location.x > Rect_left &&
		this->location.x < Rect_right &&
		this->location.y > Rect_top &&
		this->location.y < Rect_bottom)
		return false;
	return true;
}

bool RoundObject::collision_aabb(RectObject& other)
{
	double Rect_left = other.get_locationX() - other.get_width() / 2;
	double Rect_right = other.get_locationX() + other.get_width() / 2;
	double Rect_top = other.get_locationY() - other.get_height() / 2;
	double Rect_bottom = other.get_locationY() + other.get_height() / 2;

	Pos Nearest_pos(
		max(Rect_left, min(this->location.x, Rect_right)),
		max(Rect_top, min(this->location.y, Rect_bottom))
	);

	return distance(Nearest_pos, this->location) < radius / 2;
}
bool RoundObject::collision_obb(RectObject& other)
{
	double cos_otherheight = cos(other.get_rotate() * M_PI / 180) * other.get_height() / 2;
	double sin_otherheight = sin(other.get_rotate() * M_PI / 180) * other.get_height() / 2;
	Vec vec_height = { cos_otherheight, sin_otherheight }; 	//矩形沿着height方向的向量，模为other.height /2,

	//	将圆和矩形进行坐标系变换，使得矩形长宽方向沿坐标轴，便可以使用圆与aabb盒的碰撞方式处理
	double c = acos(
		dot(vec_height, Vec(0, 1)) / (double(other.get_height()) / 2)
	);
	Pos new_Roundlocation(
		this->location.x * cos(c) + this->location.y * sin(c),
		this->location.y * cos(c) - this->location.x * sin(c)
	);
	Pos new_Rectlocation(
		other.get_locationX() * cos(c) + other.get_locationY() * sin(c),
		other.get_locationY() * cos(c) - other.get_locationX() * sin(c)
	);

	RoundObject new_Round(new_Roundlocation, this->radius, this->rotate - c * 180.0 / M_PI);
	RectObject new_Rect(new_Rectlocation, other.get_width(), other.get_height());

	return new_Round.collision_aabb(new_Rect);
}

bool RoundObject::collision_aabb(RectObject& other, Pos& ori_RoundPos,/*output*/Collision_state* state_out)
{
	bool result = false;
	double Rect_left = other.get_locationX() - other.get_width() / 2;
	double Rect_right = other.get_locationX() + other.get_width() / 2;
	double Rect_top = other.get_locationY() - other.get_height() / 2;
	double Rect_bottom = other.get_locationY() + other.get_height() / 2;

	Pos Nearest_pos(
		max(Rect_left, min(this->location.x, Rect_right)),
		max(Rect_top, min(this->location.y, Rect_bottom))
	);
	result = distance(Nearest_pos, this->location) < radius / 2;

	if (result == false || state_out == nullptr) return result;

	double format_rotate = fmod(rotate, 360.0);
	vector<Collision_state> collision_map;

	/*
	*	这里将矩形膨胀，产生圆角矩形，若发生碰撞，则碰撞时圆必定落到该圆角矩形上;
	*	然后用四条边线+四个圆的形式组成该圆角矩形，判断圆碰撞时的位置;
	*	若圆碰撞时落点在膨胀边线上，则为边碰撞;
	*	若圆碰撞时落点在膨胀圆的圆弧上，则为顶点碰撞;
	*/
	Line lines[4] =
	{
		Line(Pos(Rect_left, Rect_top - radius / 2),Pos(Rect_right,Rect_top - radius / 2)),
		Line(Pos(Rect_left, Rect_bottom + radius / 2),Pos(Rect_right,Rect_bottom + radius / 2)),
		Line(Pos(Rect_left - radius / 2, Rect_top),Pos(Rect_left - radius / 2,Rect_bottom)),
		Line(Pos(Rect_right + radius / 2, Rect_top),Pos(Rect_right + radius / 2,Rect_bottom))
	};
	for (int i = 0; i < 4; i++)
	{
		//lines[i].Draw();
		Line line(location, ori_RoundPos);
		Pos cross_pos;
		bool cross_result = cross(line, lines[i], &cross_pos);
		if (cross_result == false)continue;

		if (i < 2)	//与上边/下边发生碰撞
		{
			collision_map.emplace_back(cross_pos, 180.0 - format_rotate);
		}
		else		//与左边/右边发生碰撞
		{
			collision_map.emplace_back(cross_pos, -format_rotate);
		}
		//cross_pos.Draw(false);
	}

	Pos Corner[4] =
	{
		Pos(Rect_left, Rect_top),
		Pos(Rect_right, Rect_top),
		Pos(Rect_left, Rect_bottom),
		Pos(Rect_right, Rect_bottom)
	};
	for (auto& pos : Corner)
	{
		RoundObject corner_round(pos, radius);
		//corner_round.Draw();
		Line line(location, ori_RoundPos);
		Pos collision_pos[2];
		Line_Round_Crossresult cross_result = cross(line, corner_round, collision_pos);
		switch (cross_result)
		{
		case Line_Round_Crossresult::TwoCrossPos:
		{
			for (auto& col_pos : collision_pos)
			{
				if ((col_pos.x <= Rect_right && col_pos.x >= Rect_left) || (col_pos.y <= Rect_bottom && col_pos.y >= Rect_top))
					continue;
				Vec vec((col_pos - corner_round.location).toVec());
				//double rotate_ori = asin(vec.x / vec.norm()) * 180.0 / M_PI;
				//double axis_rotate_ori = rotate_ori + 90;
				//double axis_rotate = 180 - rotate_ori - 90;
				double axis_rotate = 90.0 - asin(vec.x / vec.norm()) * 180.0 / M_PI;
				collision_map.emplace_back(col_pos, 2.0 * axis_rotate - format_rotate);
				//Pos(col_pos.x, col_pos.y).Draw(false);
			}
			break;
		}
		case Line_Round_Crossresult::OneCrossPos:
		{
			Pos& col_pos = collision_pos[0];
			if ((col_pos.x <= Rect_right && col_pos.x >= Rect_left) || (col_pos.y <= Rect_bottom && col_pos.y >= Rect_top))
				continue;
			Vec vec((col_pos - corner_round.location).toVec());
			double axis_rotate = 90.0 - asin(vec.x / vec.norm()) * 180.0 / M_PI;
			collision_map.emplace_back(col_pos, 2.0 * axis_rotate - format_rotate);
			//Pos(col_pos.x, col_pos.y).Draw(false);
			break;
		}
		case Line_Round_Crossresult::NullCrossPos:
		default:
			break;
		}
	}

	double min_distance = DBL_MAX;
	Collision_state collision_state;
	for (auto& it : collision_map)
	{
		double cur_distance = distance(it.pos, ori_RoundPos);
		if (cur_distance < min_distance)
		{
			collision_state = it;
			min_distance = cur_distance;
		}
	}

	*state_out = collision_state;
	//state_out->pos.Draw(false);
	return true;
}
bool RoundObject::collision_obb(RectObject& other, Pos& ori_RoundPos,/*output*/Collision_state* state_out)
{
	double sin_myradius = sin(this->rotate * M_PI / 180) * this->radius / 2;
	double cos_myradius = cos(this->rotate * M_PI / 180) * this->radius / 2;

	double sin_otherwidth = sin(other.get_rotate() * M_PI / 180) * other.get_width() / 2;
	double cos_otherwidth = cos(other.get_rotate() * M_PI / 180) * other.get_width() / 2;
	double cos_otherheight = cos(other.get_rotate() * M_PI / 180) * other.get_height() / 2;
	double sin_otherheight = sin(other.get_rotate() * M_PI / 180) * other.get_height() / 2;


	Vec vec[4] = {
		{ cos_myradius,sin_myradius },	//当前圆垂直速度方向的向量，模为this->radius /2,
		{ sin_myradius, -cos_myradius },	//当前圆沿着速度方向的向量，模为this->radius /2，
		{ cos_otherheight, sin_otherheight },	//另一矩形沿着height方向的向量，模为other.height /2,
		{ sin_otherwidth, -cos_otherwidth }		//另一矩形沿着width方向的向量，模为other.width /2，
	};

	//RectObject(other.get_location(), other.get_width() + this->radius, other.get_height() + this->radius, other.get_rotate()).Draw();

	//	将圆和矩形进行坐标系变换，使得矩形长宽方向沿坐标轴，便可以使用圆与aabb盒的碰撞方式处理
	double c = acos(
		dot(vec[2], Vec(0, 1)) / (double(other.get_height() / 2))
	);

	Pos new_Roundlocation(
		this->location.x * cos(c) + this->location.y * sin(c),
		this->location.y * cos(c) - this->location.x * sin(c)
	);
	Pos new_Rectlocation(
		other.get_locationX() * cos(c) + other.get_locationY() * sin(c),
		other.get_locationY() * cos(c) - other.get_locationX() * sin(c)
	);
	Pos new_ori_RoundPos(
		ori_RoundPos.x * cos(c) + ori_RoundPos.y * sin(c),
		ori_RoundPos.y * cos(c) - ori_RoundPos.x * sin(c)
	);

	RoundObject new_Round(new_Roundlocation, this->radius, this->rotate - c * 180.0 / M_PI);
	RectObject new_Rect(new_Rectlocation, other.get_width(), other.get_height());
	//new_Rect.Draw();
	//Line(new_Roundlocation, new_ori_RoundPos).Draw();

	bool result = new_Round.collision_aabb(new_Rect, new_ori_RoundPos, state_out);

	if (result == false || state_out == nullptr) return result;
	//state_out->pos.Draw(false);

	// 将碰撞结果转换回原坐标系下的结果
	Collision_state ori_state_out;
	ori_state_out.pos.x = (state_out->pos.x * cos(c) - state_out->pos.y * sin(c)) / (cos(c) * cos(c) + sin(c) * sin(c));
	ori_state_out.pos.y = (state_out->pos.y * cos(c) + state_out->pos.x * sin(c)) / (cos(c) * cos(c) + sin(c) * sin(c));
	ori_state_out.rotate = state_out->rotate + c * 180.0 / M_PI;
	//ori_state_out.pos.Draw(false);
	*state_out = ori_state_out;

	return result;
}

void Pos::Draw(bool isFill, bool redcolor)
{
	static ID2D1SolidColorBrush* pBrush;
	pBrush = redcolor == true ? Brush::pRed_Brush : Brush::pGreen_Brush;

	if (isFill)
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), 2.5, 2.5), pBrush);
	else
		pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), 2.5, 2.5), pBrush);
}
void Line::Draw(bool redcolor)
{
	static ID2D1SolidColorBrush* pBrush;
	pBrush = redcolor == true ? Brush::pRed_Brush : Brush::pGreen_Brush;
	pRenderTarget->DrawLine(D2D1::Point2F(pos[0].x, pos[0].y), D2D1::Point2F(pos[1].x, pos[1].y), pBrush);
}

void RectObject::get_lines(Line lines[4])
{
	if (lines == nullptr)return;

	double sin_width = sin(this->rotate * M_PI / 180) * this->width / 2;
	double cos_width = cos(this->rotate * M_PI / 180) * this->width / 2;
	double cos_height = cos(this->rotate * M_PI / 180) * this->height / 2;
	double sin_height = sin(this->rotate * M_PI / 180) * this->height / 2;
	Vec vec[2] = {
		{ cos_height, sin_height },		//矩形沿着height方向的向量，模为other.height /2,
		{ sin_width, -cos_width }		//矩形沿着width方向的向量，模为other.width /2，
	};

	lines[0] = Line /*top*/(
		this->location - vec[0] - vec[1],
		this->location - vec[0] + vec[1]
	);

	lines[1] = Line /*bottom*/(
		this->location + vec[0] - vec[1],
		this->location + vec[0] + vec[1]
	);
	lines[2] = Line /*left*/(
		this->location - vec[0] - vec[1],
		this->location + vec[0] - vec[1]
	);
	lines[3] = Line /*right*/(
		this->location - vec[0] + vec[1],
		this->location + vec[0] + vec[1]
	);
}

void RectObject::Draw()
{
	Line lines[4];
	get_lines(lines);
	for (auto& line : lines)
		line.Draw();
}

void RectObject::Draw(bool redcolor)
{
	Line lines[4];
	get_lines(lines);
	for (auto& line : lines)
		line.Draw(redcolor);
}

void RoundObject::Draw()
{
	pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(location.x, location.y), radius / 2, radius / 2), Brush::pRed_Brush);
}
void RoundObject::Draw(bool isFill, bool redcolor)
{
	static ID2D1SolidColorBrush* pBrush;
	pBrush = redcolor == true ? Brush::pRed_Brush : Brush::pGreen_Brush;

	if (isFill)
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(location.x, location.y), radius / 2, radius / 2), pBrush);
	else
		pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(location.x, location.y), radius / 2, radius / 2), pBrush);
}
