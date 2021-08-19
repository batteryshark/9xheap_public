#include <Windows.h>
#include "bindings_ntdll.h"
#include "binding_helpers.h"
#include "emulateheap_kernel32.h"

tRtlAllocateHeap* o_RtlAllocateHeap = NULL;
tRtlReAllocateHeap* o_RtlReAllocateHeap = NULL;
tRtlFreeHeap* o_RtlFreeHeap = NULL;
tRtlSizeHeap* o_RtlSizeHeap = NULL;

PVOID NTAPI h_RtlAllocateHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN SIZE_T Size) {
    if (_IsOurHeap(HeapHandle)) {
        return _HeapAlloc(HeapHandle, Flags, Size);
    }
    else {
        return o_RtlAllocateHeap(HeapHandle, Flags, Size);
    }
}

PVOID NTAPI h_RtlReAllocateHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN LPVOID Address, IN SIZE_T Size) {
    LPVOID uRet = FALSE;
    if (UseOurHeap(HeapHandle, Address)) {
        uRet = _HeapReAlloc(HeapHandle, Flags, Address, Size);
    }
    else {
        if (ValidateNTHeap(HeapHandle, Address)) {
            uRet = o_RtlReAllocateHeap(HeapHandle, Flags, Address, Size);
        }
    }
    return uRet;
}

BOOLEAN NTAPI h_RtlFreeHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN PVOID Address) {
    BOOL bRet = FALSE;
    if (UseOurHeap(HeapHandle, Address)) {
        bRet = _HeapFree(HeapHandle, Flags, Address);
    }
    else {
        if (ValidateNTHeap(HeapHandle, Address)) {
            bRet = o_RtlFreeHeap(HeapHandle, Flags, Address);
        }
    }
    return bRet;
}

ULONG NTAPI h_RtlSizeHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN PVOID Address) {
    BOOL bRet = FALSE;
    if (UseOurHeap(HeapHandle, Address)) {
        bRet = _HeapSize(HeapHandle, Flags, Address);
    }
    else {
        if (ValidateNTHeap(HeapHandle, Address)) {
            bRet = o_RtlSizeHeap(HeapHandle, Flags, Address);
        }
    }
    return bRet;
}