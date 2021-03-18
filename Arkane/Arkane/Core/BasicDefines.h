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


#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif