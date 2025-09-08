/*
 * 9xheap_core.c
 * 
 * Implementation of the Windows 9x heap emulation core module.
 * This provides a clean API wrapper around the original Win9x heap code.
 */

#define BUILDING_9XHEAP_CORE
#include "9xheap_core_api.h"
#include "emulateheap_kernel32.h"

/*
 * Core Heap Management Functions
 */

CORE_API BOOL Core_HeapInit(void) {
    return _HeapInit();
}

CORE_API HHEAP9X Core_HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) {
    return (HHEAP9X)_HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
}

CORE_API BOOL Core_HeapDestroy(HHEAP9X hHeap) {
    return _HeapDestroy((HANDLE)hHeap);
}

CORE_API LPVOID Core_HeapAlloc(HHEAP9X hHeap, DWORD dwFlags, SIZE_T dwBytes) {
    return HeapAlloc((HANDLE)hHeap, dwFlags, dwBytes);
}

CORE_API LPVOID Core_HeapReAlloc(HHEAP9X hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes) {
    return HeapReAlloc((HANDLE)hHeap, dwFlags, lpMem, dwBytes);
}

CORE_API BOOL Core_HeapFree(HHEAP9X hHeap, DWORD dwFlags, LPVOID lpMem) {
    return HeapFree((HANDLE)hHeap, dwFlags, lpMem);
}

CORE_API SIZE_T Core_HeapSize(HHEAP9X hHeap, DWORD dwFlags, LPVOID lpMem) {
    return HeapSize((HANDLE)hHeap, dwFlags, lpMem);
}

CORE_API BOOL Core_HeapValidate(HHEAP9X hHeap, DWORD dwFlags, LPCVOID lpMem) {
    // Win9x behavior - not implemented
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

CORE_API SIZE_T Core_HeapCompact(HHEAP9X hHeap, DWORD dwFlags) {
    // Win9x behavior - not implemented
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}

CORE_API BOOL Core_HeapLock(HHEAP9X hHeap) {
    // Win9x behavior - not implemented
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

CORE_API BOOL Core_HeapUnlock(HHEAP9X hHeap) {
    // Win9x behavior - not implemented
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

CORE_API BOOL Core_HeapWalk(HHEAP9X hHeap, LPPROCESS_HEAP_ENTRY lpEntry) {
    // Win9x behavior - not implemented
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

/*
 * Local Memory Functions (9x style)
 */

CORE_API HLOCAL Core_LocalAlloc(UINT uFlags, SIZE_T uBytes) {
    return LocalAllocNG(uFlags, uBytes);
}

CORE_API HLOCAL Core_LocalFree(HLOCAL hMem) {
    return LocalFreeNG(hMem);
}

CORE_API HLOCAL Core_LocalReAlloc(HLOCAL hMem, SIZE_T uBytes, UINT uFlags) {
    return LocalReAlloc(hMem, uBytes, uFlags);
}

CORE_API LPVOID Core_LocalLock(HLOCAL hMem) {
    return LocalLock(hMem);
}

CORE_API BOOL Core_LocalUnlock(HLOCAL hMem) {
    return LocalUnlock(hMem);
}

CORE_API SIZE_T Core_LocalSize(HLOCAL hMem) {
    return LocalSize(hMem);
}

CORE_API UINT Core_LocalFlags(HLOCAL hMem) {
    return LocalFlags(hMem);
}

CORE_API HLOCAL Core_LocalHandle(LPCVOID pMem) {
    return LocalHandle((PVOID)pMem);
}

/*
 * Heap Identification Functions
 */

CORE_API BOOL Core_IsOurHeap(HHEAP9X hHeap) {
    return _IsOurHeap((HANDLE)hHeap);
}

CORE_API BOOL Core_IsOnOurHeap(LPCVOID lpMem) {
    return _IsOnOurHeap(lpMem);
}

CORE_API BOOL Core_IsOurLocalHeap(HLOCAL hMem) {
    return _IsOurLocalHeap(hMem);
}

// Static process heap for this implementation
static HHEAP9X g_processLocalHeap = NULL;

CORE_API HHEAP9X Core_GetProcessLocalHeap(void) {
    if (!g_processLocalHeap) {
        // Create a default process heap
        g_processLocalHeap = Core_HeapCreate(0, 0x10000, 0); // 64KB initial
    }
    return g_processLocalHeap;
}

// Internal compatibility functions are provided via core_compat.h in the shim

// Static library - no DLL entry point needed
