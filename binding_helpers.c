#include <Windows.h>
#include <winternl.h>
#include "bindings_kernel32.h"
#include "bindings_ntdll.h"
#include "binding_helpers.h"
#include "emulateheap_kernel32.h"

#define RtlProcessHeap() (HANDLE)(NtCurrentTeb()->ProcessEnvironmentBlock->Reserved4[0])

BOOL UseOurHeap(HANDLE hHeap, LPCVOID lpMem){
    return (_IsOurHeap(hHeap) || ((hHeap == RtlProcessHeap()) && _IsOnOurHeap(lpMem)));
}

BOOL ValidateNTHeap(HANDLE hHeap, LPCVOID lpMem){
    BOOL bRet = FALSE;
    __try {
        bRet = o_HeapValidate(hHeap, 0, lpMem);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
    return bRet;
}