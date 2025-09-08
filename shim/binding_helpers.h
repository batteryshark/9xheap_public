#pragma once

BOOL UseOurHeap(HANDLE hHeap, LPCVOID lpMem);
BOOL ValidateNTHeap(HANDLE hHeap, LPCVOID lpMem);
HANDLE FixHeap(HANDLE hHeap, LPCVOID lpMem);
void InitializeRealProcessHeap(void);
HANDLE GetRealProcessHeap(void);
