#pragma once

#include "../Core/Types.h"
#include <set>


#pragma warning(disable : 4251)

AK_NAMESPACE_BEGIN

class AK_DLL System
{
public:
	std::set<Entity> m_entities;
};

AK_NAMESPACE_END