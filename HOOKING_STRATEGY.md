# 9xheap Minimal Hooking Strategy

## Overview
This shim uses a minimal, compatible hooking approach designed for maximum compatibility across Windows 95 through Windows 11, as well as Wine/Crossover environments.

## Hooked Functions

### Core Local Memory Functions (kernel32.dll)
- `LocalAlloc` - Primary target for legacy 16-bit heap compatibility
- `LocalFree` - Essential for proper cleanup
- `LocalReAlloc` - Memory reallocation support

### Core Global Memory Functions (kernel32.dll)
- `GlobalAlloc` - Legacy global heap support
- `GlobalFree` - Global heap cleanup
- `GlobalReAlloc` - Global heap reallocation

### Process Heap Functions (kernel32.dll)
- `GetProcessHeap` - Returns our emulated heap handle

### Low-Level NT Functions (ntdll.dll)
- `RtlAllocateHeap` - Deep integration at NT kernel level
- `RtlFreeHeap` - NT-level heap deallocation

## Compatibility Strategy

### Windows 95/98/ME Compatibility
- Uses only `kernel32.dll` exports (no `api-ms-*` dependencies)
- Avoids Windows 7+ API forwarders
- Graceful fallback if NTDLL functions don't exist

### Windows XP/2003 Compatibility
- Full support for both kernel32 and ntdll hooks
- Proper SEH handling via compatibility macros

### Windows 10/11 Compatibility
- Handles API forwarders transparently
- Works with modern heap implementations
- Compatible with CFG/CET security features

### Wine/Crossover Compatibility
- Uses standard Win32 APIs only
- No Windows-specific implementation details
- Robust error handling for missing functions

## Technical Implementation

### Hotpatching Mechanism
- Uses `x86_ldasm` for instruction length disassembly
- Creates proper trampolines to original functions
- Preserves original calling conventions
- Thread-safe initialization

### Memory Management
- Integrates with `9xheap_core.dll` for actual heap operations
- Falls back to original functions for non-9x allocations
- Maintains compatibility with existing heap handles

### Error Handling
- Checks function availability before hooking
- Graceful degradation if hooks fail
- Maintains system stability even with partial hook failures

## Build Configuration
- Compiled with MinGW for maximum compatibility
- Uses `-nostdlib` with minimal CRT
- Optimized for size and compatibility over performance
- No external dependencies beyond system DLLs
