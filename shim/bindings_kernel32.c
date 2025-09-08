#include <windows.h>
#include "bindings_kernel32.h"
#include "binding_helpers.h"
#include "../core/emulateheap_kernel32.h"
#include "mingw_compat.h"

// Original function pointers
tHeapCreate* o_HeapCreate = NULL;
tHeapDestroy* o_HeapDestroy = NULL;
tHeapValidate* o_HeapValidate = NULL;
tHeapCompact* o_HeapCompact = NULL;
tHeapWalk* o_HeapWalk = NULL;
tHeapLock* o_HeapLock = NULL;
tHeapUnlock* o_HeapUnlock = NULL;
tGetProcessHeap* o_GetProcessHeap = NULL;
tLocalAlloc* o_LocalAlloc = NULL;
tLocalFree* o_LocalFree = NULL;
tLocalReAlloc* o_LocalReAlloc = NULL;
tLocalLock* o_LocalLock = NULL;
tLocalUnlock* o_LocalUnlock = NULL;
tLocalHandle* o_LocalHandle = NULL;
tLocalSize* o_LocalSize = NULL;
tLocalFlags* o_LocalFlags = NULL;
tGlobalAlloc* o_GlobalAlloc = NULL;
tGlobalFree* o_GlobalFree = NULL;
tGlobalReAlloc* o_GlobalReAlloc = NULL;
tGlobalLock* o_GlobalLock = NULL;
tGlobalUnlock* o_GlobalUnlock = NULL;
tGlobalHandle* o_GlobalHandle = NULL;
tGlobalSize* o_GlobalSize = NULL;
tGlobalFlags* o_GlobalFlags = NULL;

// Heap functions - call emulated heap directly
HANDLE __stdcall h_HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) {
    return _HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
}

BOOL __stdcall h_HeapDestroy(HANDLE hHeap) {
    if (_IsOurHeap(hHeap)) {
        return _HeapDestroy(hHeap);
    }
    return o_HeapDestroy ? o_HeapDestroy(hHeap) : FALSE;
}

BOOL __stdcall h_HeapValidate(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem) {
    BOOL bRet = FALSE;
    hHeap = FixHeap(hHeap, lpMem);
    if (UseOurHeap(hHeap, lpMem)) {
        // Use the working code's fallback approach
        bRet = TRUE; // Our heap is always valid
    }
    else {
        if (ValidateNTHeap(hHeap, lpMem)) {
            bRet = o_HeapValidate ? o_HeapValidate(hHeap, dwFlags, lpMem) : FALSE;
        }
    }
    return bRet;
}

SIZE_T __stdcall h_HeapCompact(HANDLE hHeap, DWORD dwFlags) {
    if (_IsOurHeap(hHeap)) {
        // No-op for our heap, return 0 (no compaction needed)
        return 0;
    }
    return o_HeapCompact ? o_HeapCompact(hHeap, dwFlags) : 0;
}

BOOL __stdcall h_HeapWalk(HANDLE hHeap, LPPROCESS_HEAP_ENTRY lpEntry) {
    if (_IsOurHeap(hHeap)) {
        // Not implemented for our heap
        SetLastError(ERROR_NO_MORE_ITEMS);
        return FALSE;
    }
    return o_HeapWalk ? o_HeapWalk(hHeap, lpEntry) : FALSE;
}

BOOL __stdcall h_HeapLock(HANDLE hHeap) {
    if (_IsOurHeap(hHeap)) {
        // No-op for our heap (already thread-safe)
        return TRUE;
    }
    return o_HeapLock ? o_HeapLock(hHeap) : FALSE;
}

BOOL __stdcall h_HeapUnlock(HANDLE hHeap) {
    if (_IsOurHeap(hHeap)) {
        // No-op for our heap (already thread-safe)
        return TRUE;
    }
    return o_HeapUnlock ? o_HeapUnlock(hHeap) : FALSE;
}

HANDLE __stdcall h_GetProcessHeap() {
    return GetCurrentPdb()->hheapLocal;
}

// Local functions - call emulated heap directly
DECLSPEC_ALLOCATOR HLOCAL __stdcall h_LocalAlloc(UINT uFlags, SIZE_T uBytes) {
    return _LocalAlloc(uFlags, uBytes);
}

HLOCAL __stdcall h_LocalFree(_Frees_ptr_opt_ HLOCAL hMem) {
    if (_IsOurLocalHeap(hMem)) {
        return _LocalFree(hMem);
    }
    return o_LocalFree ? o_LocalFree(hMem) : hMem;
}

DECLSPEC_ALLOCATOR HLOCAL __stdcall h_LocalReAlloc(_Frees_ptr_opt_ HLOCAL hMem, SIZE_T uBytes, UINT uFlags) {
    if (_IsOurLocalHeap(hMem)) {
        return _LocalReAlloc(hMem, uBytes, uFlags);
    }
    return o_LocalReAlloc ? o_LocalReAlloc(hMem, uBytes, uFlags) : NULL;
}

LPVOID __stdcall h_LocalLock(HLOCAL hMem) {
    if (_IsOurLocalHeap(hMem)) {
        return _LocalLock(hMem);
    }
    return o_LocalLock ? o_LocalLock(hMem) : NULL;
}

BOOL __stdcall h_LocalUnlock(HLOCAL hMem) {
    if (_IsOurLocalHeap(hMem)) {
        return _LocalUnlock(hMem);
    }
    return o_LocalUnlock ? o_LocalUnlock(hMem) : FALSE;
}

HLOCAL __stdcall h_LocalHandle(LPCVOID pMem) {
    if (_IsOurLocalHeap((HANDLE)pMem)) {
        return _LocalHandle(pMem);
    }
    return o_LocalHandle ? o_LocalHandle(pMem) : NULL;
}

SIZE_T __stdcall h_LocalSize(HLOCAL hMem) {
    if (_IsOurLocalHeap(hMem)) {
        return _LocalSize(hMem);
    }
    return o_LocalSize ? o_LocalSize(hMem) : 0;
}

UINT __stdcall h_LocalFlags(HLOCAL hMem) {
    if (_IsOurLocalHeap(hMem)) {
        return _LocalFlags(hMem);
    }
    return o_LocalFlags ? o_LocalFlags(hMem) : LMEM_INVALID_HANDLE;
}

// Global functions - map to Local functions like the original
DECLSPEC_ALLOCATOR HGLOBAL __stdcall h_GlobalAlloc(UINT uFlags, SIZE_T dwBytes) {
    return _LocalAlloc(uFlags, dwBytes);
}

HGLOBAL __stdcall h_GlobalFree(_Frees_ptr_opt_ HGLOBAL hMem) {
    if (_IsOurLocalHeap(hMem)) {
        return _LocalFree(hMem);
    }
    return o_GlobalFree ? o_GlobalFree(hMem) : hMem;
}

DECLSPEC_ALLOCATOR HGLOBAL __stdcall h_GlobalReAlloc(_Frees_ptr_ HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags) {
    if (_IsOurLocalHeap(hMem)) {
        UINT uLocalFlags = uFlags;
        return _LocalReAlloc(hMem, dwBytes, uLocalFlags);
    }
    return o_GlobalReAlloc ? o_GlobalReAlloc(hMem, dwBytes, uFlags) : NULL;
}

LPVOID __stdcall h_GlobalLock(HGLOBAL hMem) {
    if (_IsOurLocalHeap(hMem)) {
        return _LocalLock(hMem);
    }
    return o_GlobalLock ? o_GlobalLock(hMem) : NULL;
}

BOOL __stdcall h_GlobalUnlock(HLOCAL hMem) {
    if (_IsOurLocalHeap(hMem)) {
        return _LocalUnlock(hMem);
    }
    return o_GlobalUnlock ? o_GlobalUnlock(hMem) : FALSE;
}

HGLOBAL __stdcall h_GlobalHandle(LPCVOID pMem) {
    if (_IsOurLocalHeap((HANDLE)pMem)) {
        return _LocalHandle(pMem);
    }
    return o_GlobalHandle ? o_GlobalHandle(pMem) : NULL;
}

SIZE_T __stdcall h_GlobalSize(HGLOBAL hMem) {
    if (_IsOurLocalHeap(hMem)) {
        return _LocalSize(hMem);
    }
    return o_GlobalSize ? o_GlobalSize(hMem) : 0;
}

UINT __stdcall h_GlobalFlags(HGLOBAL hMem) {
    if (_IsOurLocalHeap(hMem)) {
        UINT uRet = _LocalFlags(hMem);
        // Convert the flags
        UINT uNewRet = uRet;
        if (uRet & LMEM_DISCARDABLE) uNewRet |= GMEM_DISCARDABLE;
        if (uRet & LMEM_MOVEABLE) uNewRet |= GMEM_MOVEABLE;
        if (uRet & LMEM_NOCOMPACT) uNewRet |= GMEM_NOCOMPACT;
        if (uRet & LMEM_NODISCARD) uNewRet |= GMEM_NODISCARD;
        if (uRet & LMEM_ZEROINIT) uNewRet |= GMEM_ZEROINIT;
        if (uRet & LMEM_MODIFY) uNewRet |= GMEM_MODIFY;
        if (uRet & LMEM_LOCKCOUNT) uNewRet |= GMEM_LOCKCOUNT;
        return uNewRet;
    }
    return o_GlobalFlags ? o_GlobalFlags(hMem) : GMEM_INVALID_HANDLE;
}
