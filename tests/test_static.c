#include <windows.h>
#include <stdio.h>

// Static import declaration - just calling this triggers DLL load and hooking
__declspec(dllimport) void fix_9xheap(void);

int main() {
    printf("Testing 9xheap shim with static import...\n");
    
    // Test allocation BEFORE hooking to make sure normal allocation works
    printf("Testing normal allocation first...\n");
    HLOCAL normalMem = LocalAlloc(LMEM_FIXED, 64);
    if (normalMem) {
        printf("Normal LocalAlloc works: %p\n", normalMem);
        LocalFree(normalMem);
        printf("Normal LocalFree works\n");
    }
    
    // Call the exported function - this loads the DLL and runs DllMain
    printf("Calling fix_9xheap()...\n");
    fix_9xheap();
    
    printf("9xheap shim loaded! Now testing hooked allocation...\n");
    
    // Test that our hooks are working
    HLOCAL testMem = LocalAlloc(LMEM_FIXED, 1024);
    if (testMem) {
        printf("LocalAlloc succeeded: %p\n", testMem);
        if (LocalFree(testMem) == NULL) {
            printf("LocalFree succeeded\n");
            printf("SUCCESS: 9xheap shim is working!\n");
        } else {
            printf("ERROR: LocalFree failed\n");
            return 1;
        }
    } else {
        printf("ERROR: LocalAlloc failed\n");
        return 1;
    }
    
    return 0;
}
