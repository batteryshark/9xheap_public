/*
 * mingw_compat.h - MinGW compatibility fixes for 9xheap_shim
 * 
 * This header fixes various incompatibilities between MSVC and MinGW
 * for the shim hooking code.
 */

#ifndef MINGW_COMPAT_SHIM_H
#define MINGW_COMPAT_SHIM_H

#ifdef __GNUC__

// Fix for pragma warnings
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wmultichar"
#pragma GCC diagnostic ignored "-Wunused-parameter"

// MinGW sometimes has issues with SAL annotations
#ifndef _Frees_ptr_opt_
#define _Frees_ptr_opt_
#endif

#ifndef _Frees_ptr_
#define _Frees_ptr_
#endif

// Microsoft-specific allocation annotations not available in MinGW
#ifndef DECLSPEC_ALLOCATOR
#define DECLSPEC_ALLOCATOR
#endif

// Ensure we have the right calling conventions
#ifndef __stdcall
#define __stdcall __attribute__((stdcall))
#endif

// MSVC-style SEH macros for compatibility
#define __try _try
#define __except _except
#define __finally _finally

#endif // __GNUC__

#endif // MINGW_COMPAT_SHIM_H
