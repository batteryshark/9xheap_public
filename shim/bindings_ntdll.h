#pragma once

#include "mingw_compat.h"

typedef PVOID NTAPI tRtlAllocateHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN SIZE_T Size);
typedef PVOID NTAPI tRtlReAllocateHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN LPVOID Address, IN SIZE_T Size);
typedef BOOLEAN NTAPI tRtlFreeHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN PVOID Address);
typedef SIZE_T NTAPI tRtlSizeHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN PVOID Address);

extern tRtlAllocateHeap* o_RtlAllocateHeap;
extern tRtlReAllocateHeap* o_RtlReAllocateHeap;
extern tRtlFreeHeap* o_RtlFreeHeap;
extern tRtlSizeHeap* o_RtlSizeHeap;

PVOID NTAPI h_RtlAllocateHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN SIZE_T Size);
PVOID NTAPI h_RtlReAllocateHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN LPVOID Address, IN SIZE_T Size);
BOOLEAN NTAPI h_RtlFreeHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN PVOID Address);
SIZE_T NTAPI h_RtlSizeHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN PVOID Address);