#pragma once

#include <bitset>
#include <cstdint>

#include "../../Core/Defines.h"


AK_NAMESPACE_BEGIN


using Entity = uint32_t;
static constexpr Entity AK_ECS_MAX_ENTITIES = 100000;
static constexpr Entity InvalidEntity = (Entity)-1;

using ComponentType = uint8_t;
static constexpr ComponentType AK_ECS_MAX_COMPONENTS = static_cast<ComponentType>(256);

using Signature = std::bitset<AK_ECS_MAX_COMPONENTS>;


AK_NAMESPACE_END