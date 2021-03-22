#include "ECSManager.h"

AK_NAMESPACE_BEGIN


ECSManager& ECSManager::Instance()
{
	static ECSManager instance;
	return instance;
}


ECSManager::ECSManager() : m_componentManager(MakeUniquePtr<ComponentManager>()), m_entityManager(MakeUniquePtr<EntityManager>()), m_systemManager(MakeUniquePtr<SystemManager>())
{
}

Entity ECSManager::CreateEntity()
{
	return m_entityManager->CreateEntity();
}

void ECSManager::DestroyEntity(Entity entity)
{
	m_entityManager->DestroyEntity(entity);

	m_componentManager->EntityDestroyed(entity);

	m_systemManager->EntityDestroyed(entity);
}


AK_NAMESPACE_END