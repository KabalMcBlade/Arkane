#pragma once

#include "../../Core/Assertions.h"
#include "../../Core/SmartPointers.h"

#include "../Core/Types.h"
#include "../Systems/System.h"

#include "../../Utilities/Hash.h"

#include <memory>
#include <unordered_map>
#include <mutex>


#pragma warning(disable : 4251)	// export for stl

AK_NAMESPACE_BEGIN

class AK_DLL SystemManager final
{
public:
	template<typename T>
	SharedPtr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();
		const size_t typeNameSize = strlen(typeName);
		const uint32_t hash = akHashEx(typeName, typeNameSize);

		akAssertInline(m_systems.find(hash) == m_systems.end() && "Registering system more than once.");

		const std::lock_guard<std::mutex> lock(m_mutex);

		auto system = MakeSharedPtr<T>();
		m_systems.insert({ hash, system });
		return system;
	}

	template<typename T>
	void SetSignature(Signature _signature)
	{
		const char* typeName = typeid(T).name();
		const size_t typeNameSize = strlen(typeName);
		const uint32_t hash = akHashEx(typeName, typeNameSize);

		akAssertInline(m_systems.find(hash) != m_systems.end() && "System used before registered.");

		const std::lock_guard<std::mutex> lock(m_mutex);

		m_signatures.insert({ hash, _signature });
	}

	void EntityDestroyed(Entity _entity);
	void EntitySignatureChanged(Entity _entity, Signature _signature);

private:
	std::unordered_map<uint32_t, Signature> m_signatures;
	std::unordered_map<uint32_t, SharedPtr<System>> m_systems;
	std::mutex m_mutex;
};

AK_NAMESPACE_END