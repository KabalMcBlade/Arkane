#pragma once

#include "../../Core/Assertions.h"
#include "../../Core/SmartPointers.h"

#include "../Core/Types.h"
#include "../Systems/System.h"

#include <memory>
#include <unordered_map>


#pragma warning(disable : 4251)	// export for stl

AK_NAMESPACE_BEGIN

class AK_DLL SystemManager final
{
public:
	static SystemManager& Instance();

	template<typename T>
	SharedPtr<T> RegisterSystem()
	{
		constexpr char* typeName = typeid(T).name();
		static_assert(m_systems.find(typeName) == m_systems.end() && "Registering system more than once.");

		auto system = MakeSharedPtr<T>();
		m_systems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void SetSignature(Signature _signature)
	{
		constexpr char* typeName = typeid(T).name();
		static_assert(m_systems.find(typeName) != m_systems.end() && "System used before registered.");

		m_signatures.insert({ typeName, _signature });
	}

	void EntityDestroyed(Entity _entity);
	void EntitySignatureChanged(Entity _entity, Signature _signature);

private:
	std::unordered_map<const char*, Signature> m_signatures;
	std::unordered_map<const char*, SharedPtr<System>> m_systems;
};

AK_NAMESPACE_END