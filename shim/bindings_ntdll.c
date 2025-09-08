#include <windows.h>
#include "bindings_ntdll.h"
#include "binding_helpers.h"
#include "../core/emulateheap_kernel32.h"

tRtlAllocateHeap* o_RtlAllocateHeap = NULL;
tRtlReAllocateHeap* o_RtlReAllocateHeap = NULL;
tRtlFreeHeap* o_RtlFreeHeap = NULL;
tRtlSizeHeap* o_RtlSizeHeap = NULL;

PVOID NTAPI h_RtlAllocateHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN SIZE_T Size) {
    if (UseOurHeap(HeapHandle, NULL)) {
        return _HeapAlloc(HeapHandle, Flags, Size);
    }
    return o_RtlAllocateHeap ? o_RtlAllocateHeap(HeapHandle, Flags, Size) : NULL;
}

PVOID NTAPI h_RtlReAllocateHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN LPVOID Address, IN SIZE_T Size) {
    HeapHandle = FixHeap(HeapHandle, Address);
    if (UseOurHeap(HeapHandle, Address)) {
        return _HeapReAlloc(HeapHandle, Flags, Address, Size);
    }
    return o_RtlReAllocateHeap ? o_RtlReAllocateHeap(HeapHandle, Flags, Address, Size) : NULL;
}

BOOLEAN NTAPI h_RtlFreeHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN PVOID Address) {
    HeapHandle = FixHeap(HeapHandle, Address);
    if (UseOurHeap(HeapHandle, Address)) {
        return _HeapFree(HeapHandle, Flags, Address);
    }
    return o_RtlFreeHeap ? o_RtlFreeHeap(HeapHandle, Flags, Address) : FALSE;
}

SIZE_T NTAPI h_RtlSizeHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN PVOID Address) {
    HeapHandle = FixHeap(HeapHandle, Address);
    if (UseOurHeap(HeapHandle, Address)) {
        return _HeapSize(HeapHandle, Flags, Address);
    }
    return o_RtlSizeHeap ? o_RtlSizeHeap(HeapHandle, Flags, Address) : 0;
}
