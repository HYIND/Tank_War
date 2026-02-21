#include "Manager/MapManager.h"
#include "Manager/ResourceManager.h"

constexpr int DEF_BRICK_WIDTH = 56;
constexpr int DEF_BRICK_HEIGHT = 56;
constexpr int DEF_IRON_WIDTH = 56;
constexpr int DEF_IRON_HEIGHT = 56;

MapInfo Init_Map_1()
{
	MapInfo info;

	info.backGrounp_resname = ResName::sandBK;

	// 玩家出生点
	info.tankbirthinfos.emplace_back(Pos2(80, 280), 0, 36, 36, TankVisual::VisualState::BASIC);
	info.tankbirthinfos.emplace_back(Pos2(MapBoundary::right - 80, 280), 180.f, 36, 36, TankVisual::VisualState::BASIC);

	info.aitankbirthinfos.emplace_back(Pos2(222, 100), 90.f, 36, 36, TankVisual::VisualState::BASIC);
	info.aitankbirthinfos.emplace_back(Pos2(470, 300), -95.f, 36, 36, TankVisual::VisualState::BASIC);
	info.aitankbirthinfos.emplace_back(Pos2(777, 477), -135.f, 36, 36, TankVisual::VisualState::BASIC);

	// 墙体尺寸
	constexpr int WALL_SIZE = 46;
	constexpr int BRICK_WIDTH = WALL_SIZE;
	constexpr int BRICK_HEIGHT = WALL_SIZE;
	constexpr int IRON_WIDTH = WALL_SIZE;
	constexpr int IRON_HEIGHT = WALL_SIZE;

	// 地图中心点
	const int CENTER_X = MapBoundary::right / 2;  // 500
	const int CENTER_Y = MapBoundary::bottom / 2; // 300

	// 1. 中心两个横铁墙（水平放置，确保有通道）
	// 铁墙1：中心偏上（左中右三个铁墙）
	int topIronY = CENTER_Y - WALL_SIZE * 3;
	info.wallbirthinfos.emplace_back(
		Pos2(CENTER_X - WALL_SIZE * 2, topIronY),
		0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON
	);
	info.wallbirthinfos.emplace_back(
		Pos2(CENTER_X, topIronY),
		0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON
	);
	info.wallbirthinfos.emplace_back(
		Pos2(CENTER_X + WALL_SIZE * 2, topIronY),
		0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON
	);

	// 铁墙2：中心偏下（对称）
	int bottomIronY = CENTER_Y + WALL_SIZE * 2;
	info.wallbirthinfos.emplace_back(
		Pos2(CENTER_X - WALL_SIZE * 2, bottomIronY),
		0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON
	);
	info.wallbirthinfos.emplace_back(
		Pos2(CENTER_X, bottomIronY),
		0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON
	);
	info.wallbirthinfos.emplace_back(
		Pos2(CENTER_X + WALL_SIZE * 2, bottomIronY),
		0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON
	);

	// 2. 创建近似圆形的交错墙（确保有两个相邻砖墙）
	const int RADIUS = WALL_SIZE * 4;

	// 定义圆形上的12个点，确保有连续的砖墙
	struct CirclePoint {
		int dx, dy;
		bool isIron;
	};

	// 12个点，确保有连续的砖墙通道
	CirclePoint circlePattern[] = {
		// 上方区域 - 确保有两个相邻砖墙
		{0, -RADIUS, true},           // 北 - 铁
		{RADIUS, -RADIUS, false},     // 东北 - 砖
		{RADIUS, 0, false},           // 东 - 砖（连续砖墙）
		{RADIUS, RADIUS, true},       // 东南 - 铁

		// 右侧区域
		{RADIUS / 2, RADIUS / 2, true},   // 铁
		{0, RADIUS, false},           // 南 - 砖
		{-RADIUS / 2, RADIUS / 2, false}, // 砖（连续砖墙）

		// 下方区域
		{-RADIUS, RADIUS, true},      // 西南 - 铁
		{-RADIUS, 0, false},          // 西 - 砖
		{-RADIUS, -RADIUS, false},    // 西北 - 砖（连续砖墙）

		// 左侧区域
		{-RADIUS / 2, -RADIUS / 2, true}, // 铁
		{0, -RADIUS / 2, false},        // 砖
	};

	// 创建圆形墙体
	for (const auto& point : circlePattern) {
		Pos2 pos(CENTER_X + point.dx, CENTER_Y + point.dy);

		if (point.isIron) {
			info.wallbirthinfos.emplace_back(
				pos, 0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON
			);
		}
		else {
			info.wallbirthinfos.emplace_back(
				pos, 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK
			);
		}
	}

	// 3. 上边装饰：对称的连续墙，确保有砖墙通道
	int topY = 60;
	for (int i = 0; i < 5; i++) {
		int x = 150 + i * 180;

		// 每个装饰组：确保至少有两个砖墙相邻
		if (i % 2 == 0) {
			// 组1：砖-砖-铁 模式
			info.wallbirthinfos.emplace_back(Pos2(x - WALL_SIZE, topY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(x, topY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(x + WALL_SIZE, topY), 0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON);
		}
		else {
			// 组2：铁-砖-砖 模式
			info.wallbirthinfos.emplace_back(Pos2(x - WALL_SIZE, topY), 0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON);
			info.wallbirthinfos.emplace_back(Pos2(x, topY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(x + WALL_SIZE, topY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
		}
	}

	// 4. 下边装饰：对称但不同的连续墙
	int bottomY = MapBoundary::bottom - 60;
	for (int i = 0; i < 5; i++) {
		int x = 150 + i * 180;

		// 创建不同的连续模式
		if (i % 3 == 0) {
			// 组1：砖-砖
			info.wallbirthinfos.emplace_back(Pos2(x, bottomY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(x + WALL_SIZE, bottomY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
		}
		else if (i % 3 == 1) {
			// 组2：砖-铁-砖
			info.wallbirthinfos.emplace_back(Pos2(x - WALL_SIZE, bottomY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(x, bottomY), 0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON);
			info.wallbirthinfos.emplace_back(Pos2(x + WALL_SIZE, bottomY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
		}
		else {
			// 组3：砖-砖-砖（三个连续砖墙）
			info.wallbirthinfos.emplace_back(Pos2(x - WALL_SIZE, bottomY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(x, bottomY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(x + WALL_SIZE, bottomY), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
		}
	}

	// 5. 左边装饰：垂直的连续墙
	int leftX = 60;
	for (int i = 0; i < 4; i++) {
		int y = 120 + i * 120;

		if (i % 2 == 0) {
			// 垂直的砖-砖组合
			info.wallbirthinfos.emplace_back(Pos2(leftX, y), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(leftX, y + WALL_SIZE), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
		}
		else {
			// 垂直的砖-铁-砖组合
			info.wallbirthinfos.emplace_back(Pos2(leftX, y - WALL_SIZE), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(leftX, y), 0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON);
			info.wallbirthinfos.emplace_back(Pos2(leftX, y + WALL_SIZE), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
		}
	}

	// 6. 右边装饰：对称的垂直墙
	int rightX = MapBoundary::right - 60;
	for (int i = 0; i < 4; i++) {
		int y = 120 + i * 120;

		if (i % 2 == 0) {
			// 垂直的砖-砖组合（与左边对称）
			info.wallbirthinfos.emplace_back(Pos2(rightX, y), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(rightX, y + WALL_SIZE), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
		}
		else {
			// 垂直的砖-铁-砖组合（与左边对称）
			info.wallbirthinfos.emplace_back(Pos2(rightX, y - WALL_SIZE), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
			info.wallbirthinfos.emplace_back(Pos2(rightX, y), 0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON);
			info.wallbirthinfos.emplace_back(Pos2(rightX, y + WALL_SIZE), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
		}
	}

	// 7. 角落装饰：明确的连续墙
	// 左上角：L型，两个砖墙相邻
	info.wallbirthinfos.emplace_back(Pos2(80, 80), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(126, 80), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK); // 相邻砖墙
	info.wallbirthinfos.emplace_back(Pos2(80, 126), 0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON);

	// 右上角：对称的L型
	info.wallbirthinfos.emplace_back(Pos2(920, 80), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(874, 80), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK); // 相邻砖墙
	info.wallbirthinfos.emplace_back(Pos2(920, 126), 0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON);

	// 左下角：两个相邻砖墙
	info.wallbirthinfos.emplace_back(Pos2(80, 520), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(126, 520), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(80, 474), 0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON);

	// 右下角：对称
	info.wallbirthinfos.emplace_back(Pos2(920, 520), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(874, 520), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(920, 474), 0.f, IRON_WIDTH, IRON_HEIGHT, 999999, WallProperty::WallType::IRON);

	// 8. 通道阻挡：确保有砖墙通道
	// 左侧通道：两个垂直砖墙
	info.wallbirthinfos.emplace_back(Pos2(200, 250), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(200, 296), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(200, 342), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK); // 增加第三个形成通道

	// 右侧对称通道
	info.wallbirthinfos.emplace_back(Pos2(800, 250), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(800, 296), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(800, 342), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);

	// 9. 战略位置：中心区域的砖墙通道
	// 在圆形墙的内侧添加砖墙通道
	info.wallbirthinfos.emplace_back(Pos2(CENTER_X - WALL_SIZE * 1.5, CENTER_Y - WALL_SIZE), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(CENTER_X - WALL_SIZE * 0.5, CENTER_Y - WALL_SIZE), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);

	info.wallbirthinfos.emplace_back(Pos2(CENTER_X + WALL_SIZE * 0.5, CENTER_Y + WALL_SIZE), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);
	info.wallbirthinfos.emplace_back(Pos2(CENTER_X + WALL_SIZE * 1.5, CENTER_Y + WALL_SIZE), 0.f, BRICK_WIDTH, BRICK_HEIGHT, 60, WallProperty::WallType::BRICK);


	return info;
}

MapInfo Init_Map_Debug()
{
	MapInfo info;

	info.backGrounp_resname = ResName::sandBK;
	info.tankbirthinfos.emplace_back(Pos2(50, 280), 0, 50, 50, TankVisual::VisualState::BASIC);
	info.tankbirthinfos.emplace_back(Pos2(MapBoundary::right - 50, 280), 180.f, 50, 50, TankVisual::VisualState::BASIC);

	info.aitankbirthinfos.emplace_back(Pos2(470, 100), 90.f, 50, 50, TankVisual::VisualState::BASIC);

	info.propbirthinfos.emplace_back(Pos2(500, 300), 45.f, 66, 66, PropProperty::PropType::HEALTH_PACK);

	return info;
}

std::map<MapID, MapInfo> LoadDefMap()
{
	std::map<MapID, MapInfo> maps;
	maps[0] = Init_Map_Debug();
	maps[1] = Init_Map_1();

	return maps;
}

MapManager* MapManager::Instance()
{
	static MapManager* m_instance = new MapManager();
	return m_instance;
}

bool MapManager::isMapExist(MapID id)
{
	return _maps.find(id) != _maps.end();
}

MapInfo MapManager::getMap(MapID id)
{
	if (_maps.find(id) != _maps.end())
		return _maps[id];
	return MapInfo();
}

MapManager::MapManager()
{
	_maps = LoadDefMap();
}