#pragma once

#ifdef __linux__
#include <pthread.h>
#include <mutex>
#elif defined(_WIN32)
#include <windows.h>
#endif
#include <condition_variable>
#include <chrono>

#include "PublicShareExportMacro.h"

class PUBLICSHARE_API CriticalSectionLock
{
public:
	CriticalSectionLock();
	~CriticalSectionLock();
	bool TryEnter();
	void Enter();
	void Leave();

	// 适配std::lock_guard和condition_variable
public:
	bool try_lock();
	void lock();
	void unlock();

private:
#ifdef __linux__
	pthread_mutex_t _mutex;
	pthread_mutexattr_t _attr;
#elif defined(_WIN32)
	CRITICAL_SECTION _cs;
#endif
};

class PUBLICSHARE_API LockGuard
{
public:
	LockGuard(CriticalSectionLock& lock, bool istrylock = false);
	bool isownlock();
	~LockGuard();

	LockGuard(const LockGuard&) = delete;
	LockGuard& operator=(const LockGuard&) = delete;
	LockGuard(LockGuard&&) = delete;
	LockGuard& operator=(LockGuard&&) = delete;

	void lock();
	void unlock();

private:
	CriticalSectionLock& _lock;
	bool _isownlock;
};

class PUBLICSHARE_API ConditionVariable
{
public:
	ConditionVariable() = default;
	~ConditionVariable() = default;

	ConditionVariable(const ConditionVariable&) = delete;
	ConditionVariable& operator=(const ConditionVariable&) = delete;
	ConditionVariable(ConditionVariable&&) = delete;
	ConditionVariable& operator=(ConditionVariable&&) = delete;

	void Wait(LockGuard& lock);
	bool WaitFor(LockGuard& lock, const std::chrono::microseconds ms);
	template <class BoolFunc>
	bool WaitFor(LockGuard& lock, const std::chrono::microseconds ms, BoolFunc func)
	{
		return _cv.wait_for(lock, ms, func);
	}

	void NotifyAll();
	void NotifyOne();

private:
	std::condition_variable_any _cv;
};
