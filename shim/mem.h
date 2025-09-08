/*
 * mem.h - Memory patching and hooking utilities for 9xheap
 * 
 * Simplified version focused on 32-bit x86 hotpatching only
 */

#pragma once
#include <stddef.h>

struct HotPatch_Info {
    void* target_function_address;
    void* replacement_function_address;
    void* trampoline_address;
    size_t trampoline_size;
    unsigned char* target_original_bytes;
    size_t target_original_bytes_size;
};

// Core functions needed for hotpatching
int get_function_address(const WCHAR* lib_name, const char* func_name, void** func_address);
int hotpatch_function(void* target_function_address, void* replacement_function_address, struct HotPatch_Info* ctx, void** ptrampoline_address);
