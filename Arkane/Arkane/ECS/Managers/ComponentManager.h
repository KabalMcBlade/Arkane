#pragma once



#include "../../Core/Assertions.h"
#include "../../Core/SmartPointers.h"

#include "../Core/Types.h"
#include "../Core/ComponentArray.h"
#include "../../Utilities/Hash.h"

#include <any>
#include <memory>
#include <unordered_map>
#include <mutex>

AK_NAMESPACE_BEGIN

class ComponentManager
{
public:
	template<typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();
		const size_t typeNameSize = strlen(typeName);
		const uint32_t hash = akHashEx(typeName, typeNameSize);

		static_assert(m_componentTypes.find(hash) == m_componentTypes.end() && "Registering component type more than once.");

		m_componentTypes.insert({ hash, m_nextComponentType });
		m_componentArrays.insert({ hash, MakeSharedPtr<ComponentArray<T>>() });

		++m_nextComponentType;
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();
		const size_t typeNameSize = strlen(typeName);
		const uint32_t hash = akHashEx(typeName, typeNameSize);

		akAssertInline(m_componentTypes.find(hash) != m_componentTypes.end() && "Component not registered before use.");

		return m_componentTypes[hash];
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		GetComponentArray<T>()->Insert(entity, component);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		GetComponentArray<T>()->Remove(entity);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		return GetComponentArray<T>()->Get(entity);
	}

	void EntityDestroyed(Entity entity)
	{
		for (auto const& pair : m_componentArrays)
		{
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
	}

private:
	std::unordered_map<uint32_t, ComponentType> m_componentTypes;
	std::unordered_map<uint32_t, SharedPtr<IComponentArray>> m_componentArrays;
	ComponentType m_nextComponentType;


	template<typename T>
	SharedPtr<ComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();
		const size_t typeNameSize = strlen(typeName);
		const uint32_t hash = akHashEx(typeName, typeNameSize);

		akAssertInline(m_componentTypes.find(hash) != m_componentTypes.end() && "Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[hash]);
	}
};

AK_NAMESPACE_END
