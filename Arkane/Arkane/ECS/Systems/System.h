#pragma once

#include "../Core/Types.h"
#include <set>


AK_NAMESPACE_BEGIN

class System
{
public:
	std::set<Entity> m_entities;
};

AK_NAMESPACE_END