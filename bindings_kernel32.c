#include <Windows.h>
#include "bindings_kernel32.h"
#include "binding_helpers.h"
#include "emulateheap_kernel32.h"

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

HANDLE __stdcall h_HeapCreate(DWORD  flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) {
	return _HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
}

BOOL __stdcall h_HeapDestroy(HANDLE hHeap) {
    if (_IsOurHeap(hHeap)){return _HeapDestroy(hHeap);}
    return o_HeapDestroy(hHeap);
}

BOOL __stdcall h_HeapValidate(HANDLE  hHeap, DWORD   dwFlags, LPCVOID lpMem) {
    BOOL bRet = FALSE;

    if (UseOurHeap(hHeap, lpMem)){
        // Win9x return values
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return -1;
    }

    __try{
        bRet = o_HeapValidate(hHeap, dwFlags, lpMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER){}
    
    return bRet;
}

SIZE_T __stdcall h_HeapCompact(HANDLE hHeap, DWORD  dwFlags) {
    if (_IsOurHeap(hHeap)){
        // Win9x return values
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
    }
    return o_HeapCompact(hHeap, dwFlags);
}

BOOL __stdcall h_HeapWalk(HANDLE hHeap, LPPROCESS_HEAP_ENTRY lpEntry) {
    if (_IsOurHeap(hHeap)){
        // Win9x return values
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
    }

    return o_HeapWalk(hHeap, lpEntry);
}

BOOL __stdcall h_HeapLock(HANDLE hHeap) {
    if (_IsOurHeap(hHeap)){
        // Win9x return values
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
    }
    return o_HeapLock(hHeap);
}

BOOL __stdcall h_HeapUnlock(HANDLE hHeap) {
    if (_IsOurHeap(hHeap)){
        // Win9x return values
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
    }
    return o_HeapUnlock(hHeap);
}

HANDLE __stdcall h_GetProcessHeap() {
    return GetCurrentPdb()->hheapLocal;
}

DECLSPEC_ALLOCATOR HLOCAL __stdcall h_LocalAlloc(UINT   uFlags, SIZE_T uBytes) {
    return _LocalAlloc(uFlags, uBytes);
}

HLOCAL __stdcall h_LocalFree(_Frees_ptr_opt_ HLOCAL hMem) {
    HLOCAL hRet = NULL;
    if (_IsOurLocalHeap(hMem)){
        return _LocalFree(hMem);
    }
    __try{
        hRet = o_LocalFree(hMem);
    }__except (EXCEPTION_EXECUTE_HANDLER){}

    return hRet;
}

DECLSPEC_ALLOCATOR HLOCAL __stdcall h_LocalReAlloc(_Frees_ptr_opt_ HLOCAL hMem, SIZE_T uBytes, UINT uFlags) {
    HLOCAL hRet = NULL;

    if (_IsOurLocalHeap(hMem)){
        return _LocalReAlloc(hMem, uBytes, uFlags);
    }

    __try{
        hRet = o_LocalReAlloc(hMem, uBytes, uFlags);
    }
    __except (EXCEPTION_EXECUTE_HANDLER){}

    return hRet;
}

LPVOID __stdcall h_LocalLock(HLOCAL hMem) {
    LPVOID pRet = NULL;
    if (_IsOurLocalHeap(hMem)){
        return _LocalLock(hMem);
    }

    __try{
        pRet = o_LocalLock(hMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER){}
        
    return pRet;
}

BOOL __stdcall h_LocalUnlock(HLOCAL hMem) {
    BOOL bRet = FALSE;

    if (_IsOurLocalHeap(hMem)){
        return _LocalUnlock(hMem);
    }

    __try{
        bRet = o_LocalUnlock(hMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
       
    return bRet;
}

HLOCAL __stdcall h_LocalHandle(LPCVOID pMem) {
    HANDLE hRet = NULL;

    if (_IsOurLocalHeap((HANDLE)pMem)){
        return _LocalHandle(pMem);
    }
    __try{
            hRet = o_LocalHandle(pMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
        
    return hRet;
}

SIZE_T __stdcall h_LocalSize(HLOCAL hMem) {
    UINT uRet = 0;

    if (_IsOurLocalHeap(hMem)){
        return _LocalSize(hMem);
    }
    
    __try{
        uRet = o_LocalSize(hMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
    return uRet;
}

UINT __stdcall h_LocalFlags(HLOCAL hMem) {
    UINT uRet = 0;

    if (_IsOurLocalHeap(hMem)){
        return _LocalFlags(hMem);
    }
    
    __try{
        uRet = o_LocalFlags(hMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}

    return uRet;
}

DECLSPEC_ALLOCATOR HGLOBAL __stdcall h_GlobalAlloc(UINT uFlags, SIZE_T dwBytes) {
    uFlags = (((uFlags & GMEM_ZEROINIT) ? LMEM_ZEROINIT : 0) |
        ((uFlags & GMEM_MOVEABLE) ? LMEM_MOVEABLE : 0) |
        ((uFlags & GMEM_FIXED) ? LMEM_FIXED : 0));

    return _LocalAlloc(uFlags, dwBytes);
}

HGLOBAL __stdcall h_GlobalFree(_Frees_ptr_opt_ HGLOBAL hMem) {
    HGLOBAL hRet = NULL;

    if (_IsOurLocalHeap(hMem)){
       return _LocalFree(hMem);
    }

    __try{
        hRet = o_GlobalFree(hMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
       
    return hRet;
}

DECLSPEC_ALLOCATOR HGLOBAL __stdcall h_GlobalReAlloc(_Frees_ptr_ HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags) {
    UINT uLocalFlags =
        (((uFlags & GMEM_ZEROINIT) ? LMEM_ZEROINIT : 0) |
            ((uFlags & GMEM_MOVEABLE) ? LMEM_MOVEABLE : 0) |
            ((uFlags & GMEM_FIXED) ? LMEM_FIXED : 0));

    HLOCAL hRet = NULL;

    if (_IsOurLocalHeap(hMem)){
        return _LocalReAlloc(hMem, dwBytes, uLocalFlags);
    }

    __try{
        hRet = o_GlobalReAlloc(hMem, dwBytes, uFlags);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}

    return hRet;
}

LPVOID __stdcall h_GlobalLock(HGLOBAL hMem) {
    LPVOID pRet = NULL;

    if (_IsOurLocalHeap(hMem)){
        return _LocalLock(hMem);
    }
    
    __try{
        pRet = o_GlobalLock(hMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}

    return pRet;
}

BOOL __stdcall h_GlobalUnlock(HLOCAL hMem) {
    BOOL bRet = FALSE;

    if (_IsOurLocalHeap(hMem)){
        return _LocalUnlock(hMem);
    }
    
    __try{
        bRet = o_GlobalUnlock(hMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
        
    return bRet;
}

HGLOBAL __stdcall h_GlobalHandle(LPCVOID pMem) {
    HANDLE hRet = NULL;

    if (_IsOurLocalHeap((HANDLE)pMem)){
        return _LocalHandle(pMem);
    }

    __try{
        hRet = o_GlobalHandle(pMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}

    return hRet;
}

SIZE_T __stdcall h_GlobalSize(HGLOBAL hMem) {
    UINT uRet = 0;

    if (_IsOurLocalHeap(hMem)){
        return _LocalSize(hMem);
    }
    
    __try{
        uRet = o_GlobalSize(hMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
    return uRet;
}

UINT __stdcall h_GlobalFlags(HGLOBAL hMem) {
    UINT uRet = 0;

    if (_IsOurLocalHeap(hMem))
    {
        uRet = _LocalFlags(hMem);
        // Convert the flags
        UINT uNewRet = uRet;

        uRet = 0;

        if (uNewRet & LMEM_DISCARDABLE)
        {
            uRet |= GMEM_DISCARDABLE;
        }

        if (uNewRet & LMEM_DISCARDED)
        {
            uRet |= GMEM_DISCARDED;
        }
        return uRet;
    }
    __try{
            uRet = o_GlobalFlags(hMem);
        }
        __except (EXCEPTION_EXECUTE_HANDLER){}

    return uRet;
}

