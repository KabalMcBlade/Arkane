#include "SystemManager.h"

AK_NAMESPACE_BEGIN

void SystemManager::EntityDestroyed(Entity _entity)
{
	for (auto const& pair : m_systems)
	{
		auto const& system = pair.second;

		const std::lock_guard<std::mutex> lock(m_mutex);
		system->m_entities.erase(_entity);
	}
}

void SystemManager::EntitySignatureChanged(Entity _entity, Signature _signature)
{
	for (auto const& pair : m_systems)
	{
		auto const& type = pair.first;
		auto const& system = pair.second;
		auto const& systemSignature = m_signatures[type];

		const std::lock_guard<std::mutex> lock(m_mutex);

		if ((_signature & systemSignature) == systemSignature)
		{
			system->m_entities.insert(_entity);
		}
		else
		{
			system->m_entities.erase(_entity);
		}
	}
}

AK_NAMESPACE_END