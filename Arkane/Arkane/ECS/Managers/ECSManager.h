#pragma once

#include "../../Core/Assertions.h"
#include "../../Core/SmartPointers.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"


AK_NAMESPACE_BEGIN

class AK_DLL ECSManager
{
public:
	static ECSManager& GetInstance();

	Entity CreateEntity();
	void DestroyEntity(Entity entity);

	template<typename T>
	void RegisterComponent()
	{
		m_componentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		m_componentManager->AddComponent<T>(entity, component);

		auto signature = m_entityManager->GetSignature(entity);
		signature.set(m_componentManager->GetComponentType<T>(), true);
		m_entityManager->SetSignature(entity, signature);

		m_systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		m_componentManager->RemoveComponent<T>(entity);

		auto signature = m_entityManager->GetSignature(entity);
		signature.set(m_componentManager->GetComponentType<T>(), false);
		m_entityManager->SetSignature(entity, signature);

		m_systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		return m_componentManager->GetComponent<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return m_componentManager->GetComponentType<T>();
	}


	// System methods
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return m_systemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemSignature(Signature signature)
	{
		m_systemManager->SetSignature<T>(signature);
	}

private:
	ECSManager();

private:
	UniquePtr<ComponentManager> m_componentManager;
	UniquePtr<EntityManager> m_entityManager;
	UniquePtr<SystemManager> m_systemManager;
};

AK_NAMESPACE_END