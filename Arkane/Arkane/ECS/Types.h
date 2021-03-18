#pragma once

#include <cstdint>

using Entity = std::uint16_t;
static constexpr Entity AK_ECS_MAX_ENTITIES = 64000;

using Component = std::uint8_t;
static constexpr  Component AK_ECS_MAX_COMPONENTS = static_cast<Component>(256);