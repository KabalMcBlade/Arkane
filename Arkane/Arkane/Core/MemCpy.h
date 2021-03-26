#pragma once

#include "Defines.h"

#include <stdint.h>

#include <memory>


#if defined(__SSE4_2__) || defined(__SSE4_1__) || defined(__SSSE3__) || defined(__SSE3__) || defined(__SSE2__) || defined(_M_X64)
#include <emmintrin.h>
#endif


AK_NAMESPACE_BEGIN

namespace _private
{
#if defined(__SSE4_2__) || defined(__SSE4_1__) || defined(__SSSE3__) || defined(__SSE3__) || defined(__SSE2__) || defined(_M_X64)
	AK_INLINE void MemCpySIMD(void* _dst, const void* _src, size_t _len)
	{
		uint8_t* dst = (uint8_t*)_dst;
		const uint8_t* src = (const uint8_t*)_src;

		size_t i = 0;
		for (; i + 128 <= _len; i += 128)
		{
			__m128i d0 = _mm_load_si128((__m128i*) & src[i + 0 * 16]);
			__m128i d1 = _mm_load_si128((__m128i*) & src[i + 1 * 16]);
			__m128i d2 = _mm_load_si128((__m128i*) & src[i + 2 * 16]);
			__m128i d3 = _mm_load_si128((__m128i*) & src[i + 3 * 16]);
			__m128i d4 = _mm_load_si128((__m128i*) & src[i + 4 * 16]);
			__m128i d5 = _mm_load_si128((__m128i*) & src[i + 5 * 16]);
			__m128i d6 = _mm_load_si128((__m128i*) & src[i + 6 * 16]);
			__m128i d7 = _mm_load_si128((__m128i*) & src[i + 7 * 16]);
			_mm_stream_si128((__m128i*) & dst[i + 0 * 16], d0);
			_mm_stream_si128((__m128i*) & dst[i + 1 * 16], d1);
			_mm_stream_si128((__m128i*) & dst[i + 2 * 16], d2);
			_mm_stream_si128((__m128i*) & dst[i + 3 * 16], d3);
			_mm_stream_si128((__m128i*) & dst[i + 4 * 16], d4);
			_mm_stream_si128((__m128i*) & dst[i + 5 * 16], d5);
			_mm_stream_si128((__m128i*) & dst[i + 6 * 16], d6);
			_mm_stream_si128((__m128i*) & dst[i + 7 * 16], d7);
		}
		for (; i + 16 <= _len; i += 16)
		{
			__m128i d = _mm_load_si128((__m128i*) & src[i]);
			_mm_stream_si128((__m128i*) & dst[i], d);
		}
		for (; i + 4 <= _len; i += 4)
		{
			*(uint32_t*)&dst[i] = *(const uint32_t*)&src[i];
		}
		for (; i < _len; ++i)
		{
			dst[i] = src[i];
		}
		_mm_sfence();
	}
#endif
}


AK_INLINE void MemCpy(void* _ptr, const void* _src, size_t _len)
{
#if defined(__SSE4_2__) || defined(__SSE4_1__) || defined(__SSSE3__) || defined(__SSE3__) || defined(__SSE2__) || defined(_M_X64)
	if (AK_IS_ALIGNED(_ptr, 16) && AK_IS_ALIGNED(_src, 16))
	{
		_private::MemCpySIMD(_ptr, _src, _len);
	}
	else
	{
		std::memcpy(_ptr, _src, _len);
	}
#else
	memcpy(_ptr, _src, _len);
#endif
}

AK_NAMESPACE_END