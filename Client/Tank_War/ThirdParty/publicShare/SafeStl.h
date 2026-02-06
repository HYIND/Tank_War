#pragma once

#include <map>
#include <queue>
#include <functional>
#include <iostream>
#include "CriticalSectionLock.h"

template <typename K, typename V>
class SafeMap
{
public:
	SafeMap() {}

	~SafeMap() {}

	SafeMap(const SafeMap &rhs)
	{
		_map = rhs._map;
	}

	SafeMap &operator=(const SafeMap &rhs)
	{
		if (this == &rhs)
			return *this;
		std::lock_guard<CriticalSectionLock> lock(_lock);
		_map = rhs._map;
		return *this;
	}

	V &operator[](const K &key)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		return _map[key];
	}

	int Size()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		return _map.size();
	}

	bool IsEmpty()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		return _map.empty();
	}

	bool Insert(const K &key, const V &value)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (_map.find(key) != _map.end())
		{
			return false;
		}
		auto ret = _map.insert(std::pair<K, V>(key, value));
		return true;
	}

	void EnsureInsert(const K &key, const V &value)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		auto ret = _map.insert(std::pair<K, V>(key, value));
		// find key and cannot insert
		if (!ret.second)
		{
			_map.erase(ret.first);
			_map.insert(std::pair<K, V>(key, value));
			return;
		}
		return;
	}

	bool Find(const K &key, V &value)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);

		auto iter = _map.find(key);
		if (iter != _map.end())
		{
			value = iter->second;
			return true;
		}

		return false;
	}

	bool FindOldAndSetNew(const K &key, V &oldValue, const V &newValue)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);

		if (_map.size() > 0)
		{
			auto iter = _map.find(key);
			if (iter != _map.end())
			{
				oldValue = iter->second;
				_map.erase(iter);
				_map.insert(std::pair<K, V>(key, newValue));
				return true;
			}
		}

		return false;
	}

	void Erase(const K &key)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		_map.erase(key);
	}

	void Clear()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		_map.clear();
		return;
	}

	void EnsureCall(std::function<void(std::map<K, V> &map)> callback)
	{
		if (callback)
		{
			std::lock_guard<CriticalSectionLock> lock(_lock);
			callback(this->_map);
		}
	}

	void Lock()
	{
		_lock.Enter();
	}

	void UnLock()
	{
		_lock.Leave();
	}

	LockGuard MakeLockGuard()
	{
		return LockGuard(_lock);
	}

private:
	std::map<K, V> _map;
	mutable CriticalSectionLock _lock;
};

template <typename T>
class SafeQueue
{
private:
	std::queue<T> _queue;
	mutable CriticalSectionLock _lock;

public:
	SafeQueue() {}
	SafeQueue(SafeQueue &&other) { _queue = other._queue; }
	~SafeQueue() {}
	bool empty()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		bool result = _queue.empty();
		return result;
	}
	int size()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		int result = _queue.size();
		return result;
	}
	// 队列添加元素
	void enqueue(T &t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		_queue.emplace(t);
	}
	void enqueue(T &&t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		_queue.push(std::forward<T>(t));
	}
	// 队列取出元素
	bool dequeue(T &t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (_queue.empty())
		{
			return false;
		}
		t = std::move(_queue.front());
		_queue.pop();
		return true;
	}
	// 查看队列首元素
	bool front(T &t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (_queue.empty())
		{
			return false;
		}
		t = _queue.front();
		return true;
	}
	// 查看队列尾元素
	bool back(T &t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (_queue.empty())
		{
			return false;
		}
		t = _queue.back();
		return true;
	}
	void clear()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		while (!_queue.empty())
			_queue.pop();
	}

	void Lock()
	{
		_lock.Enter();
	}

	void UnLock()
	{
		_lock.Leave();
	}

	LockGuard MakeLockGuard()
	{
		return LockGuard(_lock);
	}
};

template <typename T>
class SafeArray
{
private:
	std::vector<T> _array;
	mutable CriticalSectionLock _lock;

public:
	SafeArray() {}
	SafeArray(SafeArray &&other)
	{
		std::lock_guard<CriticalSectionLock> lock(other._lock);
		_array = std::move(other._array);
	}
	SafeArray(const SafeArray &other)
	{
		std::lock_guard<CriticalSectionLock> lock(other._lock);
		_array = other._array;
	}
	SafeArray &operator=(const SafeArray &other)
	{
		if (this == &other)
			return *this;
		std::lock_guard<CriticalSectionLock> lock(other._lock);
		_array = other._array;
		return *this;
	}
	~SafeArray() {}
	void clear()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		_array.clear();
	}
	bool empty()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		bool result = _array.empty();
		return result;
	}
	int size()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		int result = _array.size();
		return result;
	}
	// 数组添加元素
	template <typename... Args>
	void emplace(Args &&...args)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		_array.emplace_back(std::forward<Args>(args)...);
	}

	// 数组访问元素
	bool getIndexElement(size_t index, T &t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (_array.empty() || index >= _array.size())
		{
			return false;
		}
		t = _array.at(index);
		return true;
	}
	// 数组删除元素
	bool deleteIndexElement(size_t index)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (_array.empty() || index >= _array.size())
		{
			return false;
		}
		_array.erase(_array.begin() + index);
		return true;
	}

	T &operator[](size_t index)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (index >= _array.size())
		{
			throw std::out_of_range("SafeArray index out of range");
		}
		return _array[index];
	}

	const T &operator[](size_t index) const
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (index >= _array.size())
		{
			throw std::out_of_range("SafeArray index out of range");
		}
		return _array[index];
	}

	void EnsureCall(std::function<void(std::vector<T> &array)> callback)
	{
		if (callback)
		{
			std::lock_guard<CriticalSectionLock> lock(_lock);
			callback(this->_array);
		}
	}

	void Lock()
	{
		_lock.Enter();
	}

	void UnLock()
	{
		_lock.Leave();
	}

	LockGuard MakeLockGuard()
	{
		return LockGuard(_lock);
	}
};

template <typename T>
class SafeDeQue
{
private:
	std::deque<T> _deque;
	mutable CriticalSectionLock _lock;

public:
	SafeDeQue() {}
	SafeDeQue(SafeDeQue &&other) { _deque = other._deque; }
	~SafeDeQue() {}
	bool empty()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		bool result = _deque.empty();
		return result;
	}
	int size()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		int result = _deque.size();
		return result;
	}
	// 队尾添加元素
	void enqueue_back(T &t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		_deque.emplace_back(t);
	}
	// 队首添加元素
	void enqueue_front(T &t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		_deque.emplace_front(t);
	}
	// 队首取出元素
	bool dequeue_front(T &t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (_deque.empty())
		{
			return false;
		}
		t = std::move(_deque.front());
		_deque.pop_front();
		return true;
	}
	// 查看队列首元素
	bool front(T &t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (_deque.empty())
		{
			return false;
		}
		t = _deque.front();
		return true;
	}
	// 查看队列尾元素
	bool back(T &t)
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		if (_deque.empty())
		{
			return false;
		}
		t = _deque.back();
		return true;
	}
	void clear()
	{
		std::lock_guard<CriticalSectionLock> lock(_lock);
		_deque.clear();
	}

	void Lock()
	{
		_lock.Enter();
	}

	void UnLock()
	{
		_lock.Leave();
	}

	LockGuard MakeLockGuard()
	{
		return LockGuard(_lock);
	}
};
