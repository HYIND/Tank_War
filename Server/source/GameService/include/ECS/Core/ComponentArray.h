#pragma once

#include <array>
#include <unordered_map>
#include <cassert>
#include "Types.h"
#include "ComponentTypes.h"
#include "IComponent.h"

class IComponentArray {
public:
	virtual ~IComponentArray() = default;
	virtual void removeComponent(EntityID entityId) = 0;
	virtual bool hasComponent(EntityID entityId) const = 0;
	virtual void entityDestroyed(EntityID entityId) = 0;
};


template<typename T>
class ComponentArray : public IComponentArray {

public:
	template<typename... Args>
	T& addComponent(EntityID entityId, Args&&... args)
	{
		assert(entityToIndex.find(entityId) == entityToIndex.end() &&
			"Entity already has this component!");

		uint32_t newIndex = NextIndex();
		entityToIndex[entityId] = newIndex;
		indexToEntity[newIndex] = entityId;

		componentData[newIndex] = T(std::forward<Args>(args)...);
		return componentData[newIndex];
	}

	void removeComponent(EntityID entityId)
	{
		if (!hasComponent(entityId))
			return;

		// 获取要删除元素的索引
		uint32_t indexOfRemoved = entityToIndex[entityId];
		uint32_t indexOfLast = size - 1;

		try {
			T* elementPtr = &componentData[indexOfRemoved];
			if (IComponent* com = dynamic_cast<IComponent*>(elementPtr))
				com->OnRemove();
		}
		catch (const std::bad_cast& e) {
		}

		// 将最后一个元素移动到被删除的位置
		if (indexOfRemoved != indexOfLast)
		{
			uint32_t lastEntityId = indexToEntity[indexOfLast];

			// 移动数据
			componentData[indexOfRemoved] = std::move(componentData[indexOfLast]);

			// 更新映射
			entityToIndex[lastEntityId] = indexOfRemoved;
			indexToEntity[indexOfRemoved] = lastEntityId;
		}

		// 删除旧的映射
		entityToIndex.erase(entityId);
		indexToEntity.erase(indexOfLast);

		size--;
	}

	T& getComponent(EntityID entityId) {
		assert(hasComponent(entityId) && "Entity does not have this component!");
		return componentData[entityToIndex[entityId]];
	}

	//const T& getComponent(EntityID entityId) const {
	//	assert(hasComponent(entityId) && "Entity does not have this component!");
	//	auto it = entityToIndex.find(entityId);
	//	return componentData[it->second];
	//}

	T* tryGetComponent(EntityID entityId)
	{
		if (!hasComponent(entityId))
			return nullptr;
		return &componentData[entityToIndex[entityId]];
	}

	bool hasComponent(EntityID entityId) const {
		return entityToIndex.find(entityId) != entityToIndex.end();
	}

	void entityDestroyed(EntityID entityId) override {
		if (hasComponent(entityId)) {
			removeComponent(entityId);
		}
	}

	T* getData() { return componentData.data(); }

	EntityID getSize() const { return size; }

	void clear() {
		entityToIndex.clear();
		indexToEntity.clear();
		size = 0;
	}

	class Iterator {
	private:
		ComponentArray<T>& array;
		uint32_t index;

	public:
		Iterator(ComponentArray<T>& arr, uint32_t idx) : array(arr), index(idx) {}

		T& operator*() { return array.componentData[index]; }
		Iterator& operator++() { index++; return *this; }
		bool operator!=(const Iterator& other) const { return &array != &other.array || index != other.index; }
	};

	Iterator begin() { return Iterator(*this, 0); }
	Iterator end() { return Iterator(*this, size); }

private:
	uint32_t NextIndex() {
		return size++;
	}

private:

	std::array<T, MAX_ENTITIES> componentData;				// 存储组件数据的连续数组
	std::unordered_map<EntityID, uint32_t> entityToIndex;	// 实体ID到数组索引的映射
	std::unordered_map<uint32_t, EntityID> indexToEntity;	// 数组索引到实体ID的映射

	uint32_t size = 0;// 当前大小
};