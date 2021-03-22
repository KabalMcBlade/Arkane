#pragma once


#include "../Core/Defines.h"

#include <cstdint>


AK_NAMESPACE_BEGIN

namespace _private
{
	// Source: https://gist.github.com/Lee-R/3839813
	constexpr uint32_t fnv1a_32(char const* s, size_t count)
	{
		return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u; // NOLINT (hicpp-signed-bitwise)
	}
}

constexpr uint32_t operator "" _Hash(char const* s, size_t count)
{
	return _private::fnv1a_32(s, count);
}

constexpr uint32_t akHashEx(char const* s, size_t count)
{
	return _private::fnv1a_32(s, count);
}

AK_NAMESPACE_END

#define _akHash(x) #x
#define akHash(x) _akHash(x) ## _Hash

/*
const char* Foo = "Foo";
const char* Pippo = "Pippo";
const char* Foo2 = "\"Foo\"";
const char* Pippo2 = "\"Pippo\"";

uint32_t A = akHash("Foo");
uint32_t B = akHash("Pippo");
uint32_t C = akHash(Foo);
uint32_t D = akHash(Pippo);
uint32_t E = akHash(Foo2);
uint32_t F = akHash(Pippo2);
uint32_t G = akHashEx(Foo2, strlen(Foo2));
uint32_t H = akHashEx(Pippo2, strlen(Pippo2));

A == G
B == H
everything else is different
*/