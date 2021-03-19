#include "EntityManager.h"


AK_NAMESPACE_BEGIN

EntityManager& EntityManager::Instance()
{
	static EntityManager instance;
	return instance;
}

EntityManager::EntityManager() : m_livingEntityCount(0)
{
	for (Entity entity = 0; entity < AK_ECS_MAX_ENTITIES; ++entity)
	{
		m_availableEntities.push(entity);
	}
}

Entity EntityManager::CreateEntity()
{
	akAssertReturnValue(m_livingEntityCount < AK_ECS_MAX_ENTITIES, InvalidEntity, "Too many entities in existence.");

	Entity id = m_availableEntities.front();
	m_availableEntities.pop();

	++m_livingEntityCount;

	return id;
}

void EntityManager::DestroyEntity(Entity _entity)
{
	akAssertReturnVoid(_entity < AK_ECS_MAX_ENTITIES, "Entity out of range.");

	m_signatures[_entity].reset();
	m_availableEntities.push(_entity);

	--m_livingEntityCount;
}

void EntityManager::SetSignature(Entity _entity, Signature _signature)
{
	akAssertReturnVoid(_entity < AK_ECS_MAX_ENTITIES, "Entity out of range.");

	m_signatures[_entity] = _signature;
}

Signature EntityManager::GetSignature(Entity _entity)
{
	akAssertReturnValue(_entity < AK_ECS_MAX_ENTITIES, InvalidEntity, "Entity out of range.");

	return m_signatures[_entity];
}


AK_NAMESPACE_END