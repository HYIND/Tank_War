#pragma once
#include "D2D.h"

class GIFINFO {
public:
	GIFINFO(double time, const std::vector<ID2D1Bitmap*>& VecBP);
	double getDefaultMsTime();
	std::vector<ID2D1Bitmap*>& getVecBP();
private:
	double defaultTime = 0;
	std::vector<ID2D1Bitmap*> _VecBP;
};


namespace ResName {
	const string textBK = "textBK";
	const string returnBP = "returnBP";
	const string pauseBP = "pauseBP";
	const string winBP = "winBP";
	const string failBP = "failBP";
	const string opBK = "opBK";
	const string brickWall = "brickWall";
	const string ironWall = "ironWall";
	const string sandBK = "sandBK";
	const string aidKit = "aidKit";
	const string defTank = "defTank";
	const string defBullet = "defBullet";
	const string orangeBullet = "orangeBullet";
	const string greenBullet = "greenBullet";
	const string purpleBullet = "purpleBullet";

	const string explosionGIF = "explosionGIF";
}


class ResourceManager {
public:
	static ResourceManager* Instance();
	bool InitResource();
	ID2D1Bitmap* GetBitMapRes(const string& name);
	GIFINFO* GetGIFRes(const string& name);
private:
	ResourceManager();

private:
	std::map <string, ID2D1Bitmap* > BitMapRes;
	std::map <string, GIFINFO* > GIFRes;
};

#define ResFactory ResourceManager::Instance()