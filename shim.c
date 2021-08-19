#include <Windows.h>
#include <stdio.h>
#include "bindings_ntdll.h"
#include "bindings_kernel32.h"

#include "mem.h"
#include "emulateheap_kernel32.h"
void init_shim() {
    struct HotPatch_Info ctx;
    LPVOID cf = NULL;

    // KERNEL32 Bindings
    get_function_address("kernelbase.dll", "HeapCreate", (void**)&cf);
    hotpatch_function(cf, h_HeapCreate, 8, &ctx, (void**)&o_HeapCreate);

    get_function_address("kernelbase.dll", "HeapDestroy", (void**)&cf);
    hotpatch_function(cf, h_HeapDestroy, 8, &ctx, (void**)&o_HeapDestroy);

    get_function_address("kernelbase.dll", "HeapValidate", (void**)&cf);
    hotpatch_function(cf, h_HeapValidate, 0x0E, &ctx, (void**)&o_HeapValidate);

    get_function_address("kernelbase.dll", "HeapCompact", (void**)&cf);
    hotpatch_function(cf, h_HeapCompact, 6, &ctx, (void**)&o_HeapCompact);

    get_function_address("kernel32.dll", "HeapWalk", (void**)&cf);
    hotpatch_function(cf, h_HeapWalk, 6, &ctx, (void**)&o_HeapWalk);

    get_function_address("kernelbase.dll", "HeapLock", (void**)&cf);
    hotpatch_function(cf, h_HeapLock, 8, &ctx, (void**)&o_HeapLock);

    get_function_address("kernelbase.dll", "HeapUnlock", (void**)&cf);
    hotpatch_function(cf, h_HeapUnlock, 8, &ctx, (void**)&o_HeapUnlock);

    get_function_address("kernelbase.dll", "GetProcessHeap", (void**)&cf);
    hotpatch_function(cf, h_GetProcessHeap, 6, &ctx, (void**)&o_GetProcessHeap);

    get_function_address("kernelbase.dll", "LocalAlloc", (void**)&cf);
    hotpatch_function(cf, h_LocalAlloc, 7, &ctx, (void**)&o_LocalAlloc);

    get_function_address("kernelbase.dll", "LocalFree", (void**)&cf);
    hotpatch_function(cf, h_LocalFree, 7, &ctx, (void**)&o_LocalFree);

    // This one might break...
    get_function_address("kernelbase.dll", "LocalReAlloc", (void**)&cf);
    hotpatch_function(cf, h_LocalReAlloc, 7, &ctx, (void**)&o_LocalReAlloc);
    
    get_function_address("kernelbase.dll", "LocalLock", (void**)&cf);
    hotpatch_function(cf, h_LocalLock, 7, &ctx, (void**)&o_LocalLock);

    get_function_address("kernelbase.dll", "LocalUnlock", (void**)&cf);
    hotpatch_function(cf, h_LocalUnlock, 7, &ctx, (void**)&o_LocalUnlock);

    // This one also might break...
    get_function_address("kernel32.dll", "LocalHandle", (void**)&cf);
    hotpatch_function(cf, h_LocalHandle, 7, &ctx, (void**)&o_LocalHandle);

    // This one also might break...
    get_function_address("kernel32.dll", "LocalSize", (void**)&cf);
    hotpatch_function(cf, h_LocalSize, 7, &ctx, (void**)&o_LocalSize);

    // This one also might break...
    get_function_address("kernel32.dll", "LocalFlags", (void**)&cf);
    hotpatch_function(cf, h_LocalFlags, 7, &ctx, (void**)&o_LocalFlags);

    // This one also might break...
    get_function_address("kernelbase.dll", "GlobalAlloc", (void**)&cf);
    hotpatch_function(cf, h_GlobalAlloc, 7, &ctx, (void**)&o_GlobalAlloc);

    // This one also might break...
    get_function_address("kernelbase.dll", "GlobalFree", (void**)&cf);
    hotpatch_function(cf, h_GlobalFree, 7, &ctx, (void**)&o_GlobalFree);

    get_function_address("kernel32.dll", "GlobalReAlloc", (void**)&cf);
    hotpatch_function(cf, h_GlobalReAlloc, 7, &ctx, (void**)&o_GlobalReAlloc);

    get_function_address("kernel32.dll", "GlobalLock", (void**)&cf);
    hotpatch_function(cf, h_GlobalLock, 7, &ctx, (void**)&o_GlobalLock);

    get_function_address("kernel32.dll", "GlobalUnlock", (void**)&cf);
    hotpatch_function(cf, h_GlobalUnlock, 7, &ctx, (void**)&o_GlobalUnlock);

    // This one also might break...
    get_function_address("kernel32.dll", "GlobalHandle", (void**)&cf);
    hotpatch_function(cf, h_GlobalHandle, 7, &ctx, (void**)&o_GlobalHandle);

    // This one also might break...
    get_function_address("kernel32.dll", "GlobalSize", (void**)&cf);
    hotpatch_function(cf, h_GlobalSize, 7, &ctx, (void**)&o_GlobalSize);

    // This one also might break...
    get_function_address("kernel32.dll", "GlobalFlags", (void**)&cf);
    hotpatch_function(cf, h_GlobalFlags, 7, &ctx, (void**)&o_GlobalFlags);


    // NTDLL Bindings
    get_function_address("ntdll.dll", "RtlAllocateHeap", (void**)&cf);
    hotpatch_function(cf, h_RtlAllocateHeap, 0x0A, &ctx, (void**)&o_RtlAllocateHeap);

    get_function_address("ntdll.dll", "RtlReAllocateHeap", (void**)&cf);
    hotpatch_function(cf, h_RtlReAllocateHeap, 8, &ctx, (void**)&o_RtlReAllocateHeap);

    get_function_address("ntdll.dll", "RtlSizeHeap", (void**)&cf);
    hotpatch_function(cf, h_RtlSizeHeap, 8, &ctx, (void**)&o_RtlSizeHeap);

    get_function_address("ntdll.dll", "RtlFreeHeap", (void**)&cf);
    hotpatch_function(cf, h_RtlFreeHeap, 8, &ctx, (void**)&o_RtlFreeHeap);
}

#ifdef _WINDLL
#define EXPORTABLE __declspec(dllexport)
EXPORTABLE void fix_9xheap() {}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        OutputDebugStringA("[Initializing 9xHeap...]");
        init_shim();
        OutputDebugStringA("[Initializing 9xHeap... OK!]");
        _HeapInit();
    }
    return TRUE;
}

#else
// Some Test Crap I wrote
int main() {
    HANDLE hOGHeap = HeapCreate(0, 0, 10);
    HANDLE hOPHeap = GetProcessHeap();

    HLOCAL ogLocal = LocalAlloc(0, 16);
    init_shim();
    _HeapInit();
    printf("OK!\n");
    HANDLE hHeap = h_HeapCreate(0, 0, 10);

    HLOCAL daLocal = h_LocalAlloc(0, 16);

    h_HeapValidate(hOGHeap, 0, 0);
    h_HeapValidate(hHeap, 0, 0);
    
    h_HeapCompact(hHeap, 0);
    h_HeapCompact(hOGHeap, 0);
    
    PROCESS_HEAP_ENTRY fff = { 0 };
    fff.lpData = NULL;
    h_HeapWalk(hHeap, &fff);
    fff.lpData = NULL;
    h_HeapWalk(hOGHeap, &fff);
    
    h_HeapLock(hHeap);
    h_HeapLock(hOGHeap);

    h_HeapUnlock(hHeap);
    h_HeapUnlock(hOGHeap);


    HANDLE hPHeap = h_GetProcessHeap();

    printf("hPHeap: %d hOPHeap: %d\n", hPHeap, hOPHeap);

    h_HeapDestroy(hHeap);
    h_HeapDestroy(hOGHeap);
    printf("TEST OK!\n");
    return 0;
}
#endif