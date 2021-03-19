#pragma once

#include "../../Core/Assertions.h"
#include "../../Core/SmartPointers.h"

#include "../Core/Types.h"

#include <array>
#include <queue>


#pragma warning(disable : 4251)	// export for stl

AK_NAMESPACE_BEGIN

class AK_DLL EntityManager final
{
public:
	static EntityManager& Instance();

	EntityManager();
	Entity CreateEntity();
	void DestroyEntity(Entity _entity);
	void SetSignature(Entity _entity, Signature _signature);
	Signature GetSignature(Entity _entity);

private:
	std::queue<Entity> m_availableEntities;
	std::array<Signature, AK_ECS_MAX_ENTITIES> m_signatures;
	uint32_t m_livingEntityCount;
};

AK_NAMESPACE_END