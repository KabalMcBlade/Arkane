#pragma once

#include <cstdio>
#include <cassert>
#include <assert.h>

#include "Defines.h"


#ifdef _DEBUG

#define akWarning( format, ... ) fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__);
#define akAssert( condition, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		assert(condition); \
    }
#define akAssertVoid( condition, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		return; \
    }
#define akAssertValue( condition, return_value, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		return return_value; \
    }

#else

#define akWarning( format, ... )
#define akAssert( condition, format, ... )
#define akAssertVoid( condition, format, ... )
#define akAssertValue( condition, return_value, format, ... )

#endif 


#define akAssertInline(x)                                               assert(x);
#define akAssertReturnVoid( condition, format, ... )					akAssertVoid( condition, format, __VA_ARGS__ )
#define akAssertReturnValue( condition, return_value, format, ...  )	akAssertValue( condition, return_value, format, __VA_ARGS__ )