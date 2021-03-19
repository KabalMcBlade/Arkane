#include "SystemManager.h"

AK_NAMESPACE_BEGIN

SystemManager& SystemManager::Instance()
{
	static SystemManager instance;
	return instance;
}

void SystemManager::EntityDestroyed(Entity _entity)
{
	for (auto const& pair : m_systems)
	{
		auto const& system = pair.second;

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