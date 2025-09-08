/*
 * 9xheap_core_api.h
 * 
 * Public API for the Windows 9x heap emulation core module.
 * This module provides a pure implementation of Win9x heap behavior
 * without any Windows API hooking or system integration.
 */

#ifndef _9XHEAP_CORE_API_H_
#define _9XHEAP_CORE_API_H_

#include <windows.h>
#include "mingw_compat.h"

#ifdef __cplusplus
extern "C" {
#endif

// Static library - no export/import needed
#define CORE_API

// Forward declarations
typedef struct heapinfo_s* HHEAP9X;

/*
 * Core Heap Management Functions
 */

// Initialize the 9x heap system
CORE_API BOOL Core_HeapInit(void);

// Create a new 9x heap
CORE_API HHEAP9X Core_HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);

// Destroy a 9x heap
CORE_API BOOL Core_HeapDestroy(HHEAP9X hHeap);

// Allocate memory from a 9x heap
CORE_API LPVOID Core_HeapAlloc(HHEAP9X hHeap, DWORD dwFlags, SIZE_T dwBytes);

// Reallocate memory in a 9x heap
CORE_API LPVOID Core_HeapReAlloc(HHEAP9X hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);

// Free memory from a 9x heap
CORE_API BOOL Core_HeapFree(HHEAP9X hHeap, DWORD dwFlags, LPVOID lpMem);

// Get size of a heap block
CORE_API SIZE_T Core_HeapSize(HHEAP9X hHeap, DWORD dwFlags, LPVOID lpMem);

// Validate a heap or heap block
CORE_API BOOL Core_HeapValidate(HHEAP9X hHeap, DWORD dwFlags, LPCVOID lpMem);

// Compact a heap
CORE_API SIZE_T Core_HeapCompact(HHEAP9X hHeap, DWORD dwFlags);

// Lock a heap
CORE_API BOOL Core_HeapLock(HHEAP9X hHeap);

// Unlock a heap
CORE_API BOOL Core_HeapUnlock(HHEAP9X hHeap);

// Walk heap blocks
CORE_API BOOL Core_HeapWalk(HHEAP9X hHeap, LPPROCESS_HEAP_ENTRY lpEntry);

/*
 * Local Memory Functions (9x style)
 */

// Allocate local memory
CORE_API HLOCAL Core_LocalAlloc(UINT uFlags, SIZE_T uBytes);

// Free local memory
CORE_API HLOCAL Core_LocalFree(HLOCAL hMem);

// Reallocate local memory
CORE_API HLOCAL Core_LocalReAlloc(HLOCAL hMem, SIZE_T uBytes, UINT uFlags);

// Lock local memory
CORE_API LPVOID Core_LocalLock(HLOCAL hMem);

// Unlock local memory
CORE_API BOOL Core_LocalUnlock(HLOCAL hMem);

// Get local memory size
CORE_API SIZE_T Core_LocalSize(HLOCAL hMem);

// Get local memory flags
CORE_API UINT Core_LocalFlags(HLOCAL hMem);

// Get handle from pointer
CORE_API HLOCAL Core_LocalHandle(LPCVOID pMem);

/*
 * Heap Identification Functions
 */

// Check if a heap handle belongs to our 9x heap system
CORE_API BOOL Core_IsOurHeap(HHEAP9X hHeap);

// Check if a pointer belongs to our 9x heap system
CORE_API BOOL Core_IsOnOurHeap(LPCVOID lpMem);

// Check if a local handle belongs to our 9x heap system
CORE_API BOOL Core_IsOurLocalHeap(HLOCAL hMem);

// Get the process local heap (9x style)
CORE_API HHEAP9X Core_GetProcessLocalHeap(void);

// Note: Internal compatibility functions are provided via core_compat.h in the shim

#ifdef __cplusplus
}
#endif

#endif // _9XHEAP_CORE_API_H_
