#pragma once


#ifdef ARKANE_EXPORTS
#define AK_DLL __declspec(dllexport)
#else
#define AK_DLL __declspec(dllimport)
#endif 


#if _WIN32 || _WIN64
#if _WIN64
#define AK_x64
#else
#define AK_x86
#endif
#elif __GNUC__
#if __x86_64__ || __ppc64__
#define AK_x64
#else
#define AK_x86
#endif
#else
#define AK_x86
#endif


// defines for easy namespace
#define AK_NAMESPACE_BEGIN namespace Arkane {
#define AK_NAMESPACE_END }

#define AK_USING_NAMESPACE using namespace Arkane; 

#define AK_OPTIMIZATION_OFF __pragma(optimize("",off))
#define AK_OPTIMIZATION_ON __pragma(optimize("",on))

/// forces a function to be in lined
#define AK_INLINE __forceinline

// tells the compiler to never inline a particular function
#define AK_NO_INLINE  __declspec(noinline)

// default memory align
#define AK_MEMORY_ALIGNMENT_SIZE 16

// Memory alignment
#define AK_MEMORY_ALIGNMENT(x)			__declspec(align(x))
#define AK_IS_ALIGNED(ptr, alignment)   ((std::uintptr_t)ptr & (alignment - 1)) == 0
#define AK_MEMORY_ALIGNED				AK_MEMORY_ALIGNMENT(AK_MEMORY_ALIGNMENT_SIZE)


#define AK_MEMORY_1_KB			(1u << 10u)
#define AK_MEMORY_2_KB			(1u << 11u)
#define AK_MEMORY_4_KB			(1u << 12u)
#define AK_MEMORY_8_KB			(1u << 13u)
#define AK_MEMORY_16_KB			(1u << 14u)
#define AK_MEMORY_32_KB			(1u << 15u)
#define AK_MEMORY_64_KB			(1u << 16u)
#define AK_MEMORY_128_KB		(1u << 17u)
#define AK_MEMORY_256_KB		(1u << 18u)
#define AK_MEMORY_512_KB		(1u << 19u)
#define AK_MEMORY_1_MB			(1u << 20u)
#define AK_MEMORY_2_MB			(1u << 21u)
#define AK_MEMORY_4_MB			(1u << 22u)
#define AK_MEMORY_8_MB			(1u << 23u)
#define AK_MEMORY_16_MB			(1u << 24u)
#define AK_MEMORY_32_MB			(1u << 25u)
#define AK_MEMORY_64_MB			(1u << 26u)
#define AK_MEMORY_128_MB        (1u << 27u)
#define AK_MEMORY_256_MB		(1u << 28u)
#define AK_MEMORY_512_MB		(1u << 29u)
#define AK_MEMORY_1024_MB		(1u << 30u)
#define AK_MEMORY_2048_MB		(1u << 31u)


#define AK_PI 3.141592653589793238f
#define AK_HALF_PI 1.5707963267948966192f

#define AK_VALUE_DEGTORAD  0.017453292519943295769236907684886f    // Degrees to Radians (PI/180)
#define AK_VALUE_RADTODEG  57.295779513082320876798154814105f      // Radians to Degrees (180/PI)
#define AK_DEG_TO_RAD(x)   (x * AK_VALUE_DEGTORAD)
#define AK_RAD_TO_DEG(x)   (x * AK_VALUE_RADTODEG)


#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif