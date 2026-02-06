#pragma once

#include <atomic>

#include "PublicShareExportMacro.h"

// 自旋锁
class PUBLICSHARE_API SpinLock
{
public:
	SpinLock();
	SpinLock(const SpinLock&) = delete;
	SpinLock& operator=(const SpinLock) = delete;
	bool trylock();
	void lock();
	void unlock();

private:
	std::atomic_flag _flag;
};