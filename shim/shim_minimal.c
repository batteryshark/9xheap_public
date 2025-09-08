#include <windows.h>
#include <stdio.h>
#include "mingw_compat.h"
#include "bindings_ntdll.h"
#include "bindings_kernel32.h"
#include "binding_helpers.h"
#include "mem.h"
#include "../core/emulateheap_kernel32.h"
#include "../core/9xheap_core_api.h"

// Track whether we've already initialized to avoid double-hooking
static int hooks_installed = 0;

// Implement get_kernel32_address like the old code
int get_kernel32_address(const WCHAR* lib_name, const char* func_name, void** func_address) {
    if (get_function_address(lib_name, func_name, func_address))
        return 1;
    if (get_function_address(L"kernelbase.dll", func_name, func_address))
        return 1;
    return get_function_address(L"kernel32.dll", func_name, func_address);
}

// Minimal, compatible shim initialization
void init_shim_minimal() {
    if (hooks_installed) {
        return;
    }
    
    struct HotPatch_Info ctx;
    LPVOID cf = NULL;

    InitializeRealProcessHeap();
    
    // Initialize the core heap system
    if (!Core_HeapInit()) {
        return;
    }
    
    // MINIMAL COMPATIBLE HOOKING STRATEGY
    // Focus only on core kernel32.dll functions that exist across Win95-Win11
    // Avoid api-ms-* DLLs which don't exist on older Windows
    
    // REPLICATE OLD WORKING LOGIC - Hook like the original shim.c
    
    // EXACT COPY OF WORKING SHIM - Hook API-MS DLLs like the original
    get_kernel32_address(L"api-ms-win-core-heap-l1-1-0.dll", "HeapCreate", (void**)&cf);
    hotpatch_function(cf, h_HeapCreate, &ctx, (void**)&o_HeapCreate);

    get_kernel32_address(L"api-ms-win-core-heap-l1-1-0.dll", "HeapDestroy", (void**)&cf);
    hotpatch_function(cf, h_HeapDestroy, &ctx, (void**)&o_HeapDestroy);

    get_kernel32_address(L"api-ms-win-core-heap-l1-1-0.dll", "HeapValidate", (void**)&cf);
    hotpatch_function(cf, h_HeapValidate, &ctx, (void**)&o_HeapValidate);

    get_kernel32_address(L"api-ms-win-core-heap-l1-1-0.dll", "HeapCompact", (void**)&cf);
    hotpatch_function(cf, h_HeapCompact, &ctx, (void**)&o_HeapCompact);

    get_kernel32_address(L"api-ms-win-core-heap-l1-1-0.dll", "HeapWalk", (void**)&cf);
    hotpatch_function(cf, h_HeapWalk, &ctx, (void**)&o_HeapWalk);

    get_kernel32_address(L"api-ms-win-core-heap-l1-1-0.dll", "HeapLock", (void**)&cf);
    hotpatch_function(cf, h_HeapLock, &ctx, (void**)&o_HeapLock);

    get_kernel32_address(L"api-ms-win-core-heap-l1-1-0.dll", "HeapUnlock", (void**)&cf);
    hotpatch_function(cf, h_HeapUnlock, &ctx, (void**)&o_HeapUnlock);

    get_kernel32_address(L"api-ms-win-core-heap-l1-1-0.dll", "GetProcessHeap", (void**)&cf);
    hotpatch_function(cf, h_GetProcessHeap, &ctx, (void**)&o_GetProcessHeap);

    // NOTE: Working code does NOT hook HeapAlloc/HeapFree - only Rtl* versions

    get_kernel32_address(L"api-ms-win-core-misc-l1-1-0.dll", "LocalAlloc", (void**)&cf);
    hotpatch_function(cf, h_LocalAlloc, &ctx, (void**)&o_LocalAlloc);

    get_kernel32_address(L"api-ms-win-core-misc-l1-1-0.dll", "LocalFree", (void**)&cf);
    hotpatch_function(cf, h_LocalFree, &ctx, (void**)&o_LocalFree);

    get_kernel32_address(L"api-ms-win-core-misc-l1-1-0.dll", "LocalReAlloc", (void**)&cf);
    hotpatch_function(cf, h_LocalReAlloc, &ctx, (void**)&o_LocalReAlloc);
    
    get_kernel32_address(L"api-ms-win-core-misc-l1-1-0.dll", "LocalLock", (void**)&cf);
    hotpatch_function(cf, h_LocalLock, &ctx, (void**)&o_LocalLock);

    get_kernel32_address(L"api-ms-win-core-misc-l1-1-0.dll", "LocalUnlock", (void**)&cf);
    hotpatch_function(cf, h_LocalUnlock, &ctx, (void**)&o_LocalUnlock);

    get_kernel32_address(L"kernel32.dll", "LocalHandle", (void**)&cf);
    hotpatch_function(cf, h_LocalHandle, &ctx, (void**)&o_LocalHandle);

    get_kernel32_address(L"api-ms-win-core-heap-obsolete-l1-1-0.dll", "LocalSize", (void**)&cf);
    hotpatch_function(cf, h_LocalSize, &ctx, (void**)&o_LocalSize);

    get_kernel32_address(L"api-ms-win-core-heap-obsolete-l1-1-0.dll", "LocalFlags", (void**)&cf);
    hotpatch_function(cf, h_LocalFlags, &ctx, (void**)&o_LocalFlags);

    get_kernel32_address(L"api-ms-win-core-misc-l1-1-0.dll", "GlobalAlloc", (void**)&cf);
    hotpatch_function(cf, h_GlobalAlloc, &ctx, (void**)&o_GlobalAlloc);

    get_kernel32_address(L"api-ms-win-core-misc-l1-1-0.dll", "GlobalFree", (void**)&cf);
    hotpatch_function(cf, h_GlobalFree, &ctx, (void**)&o_GlobalFree);

    get_kernel32_address(L"api-ms-win-core-heap-obsolete-l1-1-0.dll", "GlobalReAlloc", (void**)&cf);
    hotpatch_function(cf, h_GlobalReAlloc, &ctx, (void**)&o_GlobalReAlloc);

    get_kernel32_address(L"api-ms-win-core-heap-obsolete-l1-1-0.dll", "GlobalLock", (void**)&cf);
    hotpatch_function(cf, h_GlobalLock, &ctx, (void**)&o_GlobalLock);

    get_kernel32_address(L"api-ms-win-core-heap-obsolete-l1-1-0.dll", "GlobalUnlock", (void**)&cf);
    hotpatch_function(cf, h_GlobalUnlock, &ctx, (void**)&o_GlobalUnlock);

    get_kernel32_address(L"api-ms-win-core-heap-obsolete-l1-1-0.dll", "GlobalHandle", (void**)&cf);
    hotpatch_function(cf, h_GlobalHandle, &ctx, (void**)&o_GlobalHandle);

    get_kernel32_address(L"api-ms-win-core-heap-obsolete-l1-1-0.dll", "GlobalSize", (void**)&cf);
    hotpatch_function(cf, h_GlobalSize, &ctx, (void**)&o_GlobalSize);

    get_kernel32_address(L"api-ms-win-core-heap-obsolete-l1-1-0.dll", "GlobalFlags", (void**)&cf);
    hotpatch_function(cf, h_GlobalFlags, &ctx, (void**)&o_GlobalFlags);

    // NTDLL Bindings - exactly like old code
    get_function_address(L"ntdll.dll", "RtlAllocateHeap", (void**)&cf);
    hotpatch_function(cf, h_RtlAllocateHeap, &ctx, (void**)&o_RtlAllocateHeap);

    get_function_address(L"ntdll.dll", "RtlReAllocateHeap", (void**)&cf);
    hotpatch_function(cf, h_RtlReAllocateHeap, &ctx, (void**)&o_RtlReAllocateHeap);

    get_function_address(L"ntdll.dll", "RtlSizeHeap", (void**)&cf);
    hotpatch_function(cf, h_RtlSizeHeap, &ctx, (void**)&o_RtlSizeHeap);

    get_function_address(L"ntdll.dll", "RtlFreeHeap", (void**)&cf);
    hotpatch_function(cf, h_RtlFreeHeap, &ctx, (void**)&o_RtlFreeHeap);
    
    // Initialize the heap system EXACTLY like the working code
    _HeapInit();

    hooks_installed = 1;
}


// Simple exported function for static linking - hooks are installed automatically
__declspec(dllexport) void fix_9xheap() {
    // No-op - hooks are installed automatically on DLL load
}
