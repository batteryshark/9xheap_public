#include <windows.h>
#include <winternl.h>
#include "mingw_compat.h"
#include "bindings_kernel32.h"
#include "bindings_ntdll.h"
#include "binding_helpers.h"
#include "../core/emulateheap_kernel32.h"

// Store the real process heap handle - initialized before hooking
static HANDLE g_RealProcessHeap = NULL;

void InitializeRealProcessHeap() {
    if (!g_RealProcessHeap) {
        // Get the real process heap before we hook GetProcessHeap
        // Try the documented way first
        g_RealProcessHeap = GetProcessHeap();
        
        // If that fails or we're already hooked, try PEB access as fallback
        if (!g_RealProcessHeap) {
            // Try both possible locations in Reserved4 array
            HANDLE heap1 = (HANDLE)(NtCurrentTeb()->ProcessEnvironmentBlock->Reserved4[0]);
            HANDLE heap2 = (HANDLE)(NtCurrentTeb()->ProcessEnvironmentBlock->Reserved4[1]);
            
            // Use whichever one validates successfully
            if (HeapValidate(heap1, 0, NULL)) {
                g_RealProcessHeap = heap1;
            } else if (HeapValidate(heap2, 0, NULL)) {
                g_RealProcessHeap = heap2;
            }
        }
    }
}

HANDLE GetRealProcessHeap() {
    if (!g_RealProcessHeap) {
        InitializeRealProcessHeap();
    }
    return g_RealProcessHeap;
}

BOOL UseOurHeap(HANDLE hHeap, LPCVOID lpMem){
    return (_IsOurHeap(hHeap) || ((hHeap == GetRealProcessHeap()) && _IsOnOurHeap(lpMem)));
}

BOOL ValidateNTHeap(HANDLE hHeap, LPCVOID lpMem){
    BOOL bRet = FALSE;
    if (o_HeapValidate) {
        bRet = o_HeapValidate(hHeap, 0, lpMem);
    }
    return bRet;
}

HANDLE FixHeap(HANDLE hHeap, LPCVOID lpMem){
    if((hHeap == h_GetProcessHeap()) && !_IsOnOurHeap(lpMem) && ValidateNTHeap(GetRealProcessHeap(), lpMem))
        return GetRealProcessHeap();
    return hHeap;
}
