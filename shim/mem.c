/*
 * mem.c - Memory patching and hooking utilities for 9xheap
 * 
 * Simplified version focused on 32-bit x86 hotpatching only
 */

#include <windows.h>
#include <stddef.h>
#include <stdint.h>
#include "mingw_compat.h"
#include "mem.h"
#include "x86_ldasm_precompiled.h"

#define PAGE_SIZE 0x1000
#define HOTPATCH_ADDRESS_OFFSET 1

// 32-bit x86 hotpatch stub: push address; ret
static unsigned char hotpatch_stub[] = {
    0x68, 0x00, 0x00, 0x00, 0x00, // push [Abs Jump Address]
    0xC3                          // ret
};

int get_function_address(const WCHAR* lib_name, const char* func_name, void** func_address) {
    if (!func_address) { 
        return 0; 
    }
    *func_address = NULL;

    HMODULE hl = GetModuleHandleW(lib_name);
    if (!hl) {
        // Force load the library if it hasn't been loaded already.
        hl = LoadLibraryW(lib_name);
    }
    if (!hl) { 
        return 0; 
    }
    
    *func_address = GetProcAddress(hl, func_name);
    return (*func_address != NULL) ? 1 : 0;
}

static int alloc_executable_page(void** page_addr) {
    *page_addr = VirtualAlloc(NULL, PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    return (*page_addr != NULL) ? 1 : 0;
}

static int patch_memory(void* target_addr, void* data_ptr, size_t data_len) {
    DWORD old_prot;
    
    // Make target writable
    if (!VirtualProtect(target_addr, data_len, PAGE_EXECUTE_READWRITE, &old_prot)) {
        return 0;
    }
    
    // Copy the patch
    memcpy(target_addr, data_ptr, data_len);
    
    // Restore original protection
    VirtualProtect(target_addr, data_len, old_prot, &old_prot);
    
    return 1;
}

int hotpatch_function(void* target_function_address, void* replacement_function_address, struct HotPatch_Info* ctx, void** ptrampoline_address) {
    if (!ptrampoline_address || !target_function_address || !replacement_function_address) { 
        return 0; 
    }

    unsigned char hook_stub[sizeof(hotpatch_stub)] = { 0 };
    unsigned char trampoline_stub[sizeof(hotpatch_stub)] = { 0 };

    memcpy(hook_stub, hotpatch_stub, sizeof(hotpatch_stub));
    memcpy(trampoline_stub, hotpatch_stub, sizeof(hotpatch_stub));
    
    ctx->target_function_address = target_function_address;
    ctx->replacement_function_address = replacement_function_address;
    
    // Calculate how many bytes we need to overwrite (at least stub size)
    int i;
    for (i = 0; i < sizeof(hotpatch_stub);) {
        i += x86_ldasm(NULL, 32, (unsigned char *)target_function_address + i);
    }
    ctx->target_original_bytes_size = i;

    // Allocate space for original bytes
    ctx->target_original_bytes = VirtualAlloc(NULL, ctx->target_original_bytes_size, MEM_COMMIT, PAGE_READWRITE);
    if (!ctx->target_original_bytes) {
        return 0;
    }

    // Create hook stub - insert replacement function address
    memcpy(hook_stub + HOTPATCH_ADDRESS_OFFSET, &replacement_function_address, sizeof(void*));
    
    // Backup original instructions
    memcpy(ctx->target_original_bytes, target_function_address, ctx->target_original_bytes_size);

    // Allocate executable page for trampoline
    if (!alloc_executable_page(ptrampoline_address)) { 
        return 0; 
    }
    ctx->trampoline_address = *ptrampoline_address;
    
    // Write stolen instructions to trampoline
    memcpy(ctx->trampoline_address, ctx->target_original_bytes, ctx->target_original_bytes_size);

    // Calculate return address (after our hook stub)
    unsigned char* return_address = (unsigned char*)target_function_address + ctx->target_original_bytes_size;
    memcpy(trampoline_stub + HOTPATCH_ADDRESS_OFFSET, &return_address, sizeof(void*));

    // Write return stub to trampoline
    memcpy((unsigned char*)ctx->trampoline_address + ctx->target_original_bytes_size, trampoline_stub, sizeof(hotpatch_stub));

    ctx->trampoline_size = ctx->target_original_bytes_size + sizeof(hotpatch_stub);

    // Install the hook
    if (!patch_memory(target_function_address, hook_stub, sizeof(hotpatch_stub))) { 
        return 0; 
    }

    return 1;
}
