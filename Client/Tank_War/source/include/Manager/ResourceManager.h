#pragma once

#include "RenderEngine/D2DTools.h"
#include <map>
#include <iostream>

namespace ResName 
{
	const std::string textBK = "textBK";
	const std::string returnBP = "returnBP";
	const std::string pauseBP = "pauseBP";
	const std::string winBP = "winBP";
	const std::string failBP = "failBP";
	const std::string opBK = "opBK";
	const std::string brickWall = "brickWall";
	const std::string ironWall = "ironWall";
	const std::string sandBK = "sandBK";
	const std::string aidKit = "aidKit";
	const std::string defTank = "defTank";
	const std::string defBullet = "defBullet";
	const std::string orangeBullet = "orangeBullet";
	const std::string greenBullet = "greenBullet";
	const std::string purpleBullet = "purpleBullet";

	const std::string explosionGIF = "explosionGIF";
}

class GIFINFO {
public:
	GIFINFO(float mstime, UINT frameCount, IWICBitmapDecoder* pDecoder, IWICStream* pStream);
	~GIFINFO();
	float getDefaultMsTime();
	ID2D1Bitmap* getFrame(UINT frameNum);
	UINT getFrameCount();
private:
	float defaultTime = 0;
	UINT totalFrameCount = 0;
	IWICBitmapDecoder* pDecoder = NULL;
	IWICStream* pStream = NULL;
	std::map<UINT, ID2D1Bitmap*> _BitMaps;
};

class ResourceManager {
public:
	static ResourceManager* Instance();
	bool InitResource();
	ID2D1Bitmap* GetBitMapRes(const std::string& name);
	GIFINFO* GetGIFRes(const std::string& name);
private:
	ResourceManager();

private:
	std::map <std::string, ID2D1Bitmap* > BitMapRes;
	std::map <std::string, GIFINFO* > GIFRes;
};

#define ResFactory ResourceManager::Instance()