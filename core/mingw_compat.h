/*
 * mingw_compat.h - MinGW compatibility fixes for 9xheap_core
 * 
 * This header fixes various incompatibilities between MSVC and MinGW
 * for the Win9x heap emulation code.
 */

#ifndef MINGW_COMPAT_H
#define MINGW_COMPAT_H

#ifdef __GNUC__

// Fix for const qualifier warnings
#ifdef LPCVOID
#undef LPCVOID
#define LPCVOID PVOID
#endif

// Fix for lvalue assignment errors in emulateheap_heap.c
// These are ugly pointer casts that MSVC allows but GCC doesn't
#define CAST_LVALUE_FIX(type, expr) (*((type*)&(expr)))

// MinGW doesn't like the __declspec(dllexport) syntax in some contexts
#ifdef BUILDING_9XHEAP_CORE
#undef CORE_API
#define CORE_API __attribute__((dllexport))
#endif

// Fix for pragma warnings
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wmultichar"

// SEH compatibility - MinGW doesn't support structured exception handling
// Replace with simple if statements for basic functionality
#define _try if (1)
#define _except(x) else if (0)
#define _finally if (1)

#endif // __GNUC__

#endif // MINGW_COMPAT_H
