#pragma once

#include "../../Core/Assertions.h"
#include "../../Core/SmartPointers.h"

#include "../Core/Types.h"

#include <array>
#include <queue>
#include <mutex>
#include <unordered_map>

#pragma warning(disable : 4251)	// export for stl


AK_NAMESPACE_BEGIN

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};


template<typename T>
class ComponentArray : public IComponentArray
{
public:
	void Insert(Entity entity, T component)
	{
		akAssertInline(m_entityToIndexMap.find(entity) == m_entityToIndexMap.end() && "Component added to same entity more than once.");

		const std::lock_guard<std::mutex> lock(m_mutex);

		size_t newIndex = m_size;
		m_entityToIndexMap[entity] = newIndex;
		m_indexToEntityMap[newIndex] = entity;
		m_componentArray[newIndex] = component;
		++m_size;
	}

	void Remove(Entity entity)
	{
		akAssertInline(m_entityToIndexMap.find(entity) != m_entityToIndexMap.end() && "Removing non-existent component.");

		const std::lock_guard<std::mutex> lock(m_mutex);

		size_t indexOfRemovedEntity = m_entityToIndexMap[entity];
		size_t indexOfLastElement = m_size - 1;
		m_componentArray[indexOfRemovedEntity] = m_componentArray[indexOfLastElement];

		Entity entityOfLastElement = m_indexToEntityMap[indexOfLastElement];
		m_entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		m_indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		m_entityToIndexMap.erase(entity);
		m_indexToEntityMap.erase(indexOfLastElement);

		--m_size;
	}

	T& Get(Entity entity)
	{
		akAssertInline(m_entityToIndexMap.find(entity) != m_entityToIndexMap.end() && "Retrieving non-existent component.");

		return m_componentArray[m_entityToIndexMap[entity]];
	}

	void EntityDestroyed(Entity entity) override
	{
		if (m_entityToIndexMap.find(entity) != m_entityToIndexMap.end())
		{
			Remove(entity);
		}
	}

private:
	// is on the stack, is contiguous, is packed
	std::array<T, AK_ECS_MAX_ENTITIES> m_componentArray;
	std::unordered_map<Entity, size_t> m_entityToIndexMap;
	std::unordered_map<size_t, Entity> m_indexToEntityMap;
	std::mutex m_mutex;
	size_t m_size;
};

AK_NAMESPACE_END