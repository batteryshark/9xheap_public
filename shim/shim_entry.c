/*
 * shim_entry.c - DLL entry point for 9xheap_shim
 * 
 * Minimal entry point that initializes the heap hooking system
 */

#include <windows.h>
#include "mingw_compat.h"

// Forward declarations
extern void init_shim_minimal(void);

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL);
        init_shim_minimal();
        break;
    case DLL_PROCESS_DETACH:
        // Cleanup could go here if needed
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        // We don't need per-thread initialization
        break;
    }
    return TRUE;
}

// Entry point for DLL without CRT
BOOL WINAPI _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return DllMain(hinstDLL, fdwReason, lpvReserved);
}
