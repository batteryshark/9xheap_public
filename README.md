# 9xheap - Windows 95-XP Heap Emulation for Modern Systems

A modern, robust implementation of Windows 95/98/ME heap behavior that works seamlessly on Windows XP through Windows 11, as well as Wine/Crossover environments.

## What is 9xheap?

9xheap provides Windows 95-style heap emulation for legacy applications that depend on specific Win9x memory management behaviors. This includes proper handling of local/global memory functions, heap fragmentation patterns, and compatibility quirks that modern Windows versions have deprecated.

## Major Improvements in This Version

This is a complete rewrite of the original 9xheap project with significant architectural and reliability improvements:

### 🔧 **Build System Modernization**
- **Before**: MSVC-based build system with Visual Studio project files
- **After**: GCC MinGW cross-compilation with unified Makefile
- **Benefits**: Easier maintenance, better cross-platform support, no Visual Studio dependency

### 🎯 **Robust Function Hooking**
- **Before**: Static byte-count trampolines that broke on Windows updates
- **After**: Dynamic instruction parsing using `x86_ldasm_precompiled.h`
- **Benefits**: Automatic adaptation to different function prologues, immune to Windows updates

### 🧠 **Intelligent Process Heap Detection**
- **Before**: Brittle `ProcessEnvironmentBlock->Reserved4[0]` access with no validation
- **After**: Smart multi-fallback detection in `InitializeRealProcessHeap()`
- **Benefits**: Robust heap detection across all Windows versions with proper validation

### 📦 **Modular Architecture**
- **Before**: Monolithic hooking code mixed with heap emulation
- **After**: Clean separation into `core/` (static library) and `shim/` (hooking DLL)
- **Benefits**: Reusable heap emulation core, easier testing, future-proof design

### 🔗 **Enhanced API Resolution**
- **Before**: Direct kernel32.dll hooking only
- **After**: Multi-tier fallback: `apiset` → `kernelbase` → `kernel32`
- **Benefits**: Better compatibility with Windows 10/11 API forwarding, works on more systems

## Architecture

```
9xheap/
├── core/           # Microsoft's heap emulation code (static library)
│   ├── 9xheap_core_api.h      # Public API for heap operations
│   ├── 9xheap_core.c          # Core heap management
│   ├── emulateheap_heap.c     # Win9x heap behavior emulation
│   ├── emulateheap_lmem.c     # Local memory functions
│   └── emulateheap_support.c  # Supporting utilities
│
├── shim/           # Hooking and system integration (DLL)
│   ├── binding_helpers.c      # Smart heap detection & validation
│   ├── bindings_kernel32.c    # kernel32.dll function hooks
│   ├── bindings_ntdll.c       # ntdll.dll function hooks
│   ├── shim_minimal.c         # Minimal hooking engine
│   ├── x86_ldasm_precompiled.h # Dynamic instruction parser
│   └── shim_entry.c           # DLL entry point
│
└── tests/          # Test suite
    └── test_static.c          # Static linking validation
```

### Core Library (`lib9xheap_core.a`)
Pure implementation of Windows 9x heap behavior without any system hooks or dependencies. This allows the heap emulation to be used in other contexts or with different hooking mechanisms.

### Shim DLL (`9xheap.dll`)
Lightweight hooking layer that intercepts Windows API calls and redirects them to the core heap implementation when appropriate. Falls back to original system functions for non-9x allocations.

## Compatibility

### Supported Platforms
- **Windows 95/98/ME**: Native compatibility
- **Windows XP/2003**: Full support with both kernel32 and ntdll hooks
- **Windows Vista/7/8/8.1**: Complete compatibility
- **Windows 10/11**: Full support including latest builds
- **Wine/Crossover**: Tested and working

### Compatibility Features
- No dependencies on modern Windows APIs
- Graceful degradation if advanced features unavailable
- Thread-safe initialization and operation
- Compatible with security features (CFG/CET)
- Works with both 32-bit and 64-bit Windows (targets 32-bit applications)

## Building

### Prerequisites
- MinGW-w64 cross-compiler: `i686-w64-mingw32-gcc`
- Make

### Ubuntu/Debian
```bash
sudo apt install gcc-mingw-w64-i686
make
```

### Windows (WSL)
```bash
# Install WSL Ubuntu, then:
sudo apt update
sudo apt install gcc-mingw-w64-i686 make
make
```

### Build Output
- `dist/9xheap.dll` - Main shim DLL
- `build/lib9xheap_core.a` - Core heap library
- `build/lib9xheap.dll.a` - Import library

## Usage

### Method 1: Static Import (Recommended)
Link against the import library and call the exported function:

```c
// Link with lib9xheap.dll.a
__declspec(dllimport) void fix_9xheap(void);

int main() {
    fix_9xheap();  // Loads DLL and enables hooks
    
    // Your legacy code here - heap functions now use 9x behavior
    HLOCAL mem = LocalAlloc(LMEM_FIXED, 1024);
    // ...
}
```

### Method 2: Dynamic Loading
```c
HMODULE h9x = LoadLibrary("9xheap.dll");
if (h9x) {
    void (*fix_fn)(void) = GetProcAddress(h9x, "fix_9xheap");
    if (fix_fn) fix_fn();
}
```

### Method 3: DLL Injection
Use any DLL injection method to load `9xheap.dll` into the target process.

## Hooked Functions

### Local/Global Memory (kernel32.dll)
- `LocalAlloc`, `LocalFree`, `LocalReAlloc`
- `GlobalAlloc`, `GlobalFree`, `GlobalReAlloc`

### Process Heap (kernel32.dll)
- `GetProcessHeap` - Returns emulated heap handle

### NT Heap Functions (ntdll.dll)
- `RtlAllocateHeap`, `RtlFreeHeap` - Deep NT-level integration

## Technical Details

### Smart Heap Detection
The `InitializeRealProcessHeap()` function uses a multi-stage approach:

1. **Primary**: Call `GetProcessHeap()` before hooking
2. **Fallback 1**: Try `ProcessEnvironmentBlock->Reserved4[0]` with validation
3. **Fallback 2**: Try `ProcessEnvironmentBlock->Reserved4[1]` with validation
4. **Validation**: Use `HeapValidate()` to confirm heap handles are valid

### Dynamic Instruction Parsing
Uses `x86_ldasm` to:
- Parse function prologues dynamically
- Create proper trampolines of correct length
- Adapt to different Windows versions automatically
- Preserve original calling conventions

### Memory Management Strategy
- Detects whether allocations belong to 9x heap system
- Falls back to original Windows functions for non-9x memory
- Maintains compatibility with existing heap handles
- Thread-safe operation with minimal performance impact

## Testing

```bash
make test
```

The test suite validates:
- Normal allocation before hooking
- Hook installation success
- 9x heap behavior after hooking
- Fallback to original functions
- Memory leak detection

## Troubleshooting

### Common Issues

**DLL fails to load**
- Ensure all dependencies (kernel32.dll, ntdll.dll) are available
- Check that the target process is 32-bit
- Verify Windows version compatibility

**Hooks not working**
- Check that `fix_9xheap()` was called successfully
- Verify the application is using hooked functions (not static CRT)
- Enable debug output to trace hook installation

**Memory corruption**
- Ensure mixed heap usage isn't occurring (9x heap with system free)
- Check that the real process heap was detected correctly
- Validate heap handles before use

### Debug Mode
Rebuild with debug flags for detailed logging:
```bash
make CFLAGS="-DDEBUG -g"
```

## License

This project contains code from multiple sources:

- **Microsoft heap emulation code**: Original Windows 95 compatibility code
- **x86_ldasm**: Apache License 2.0 (instruction length disassembler)
- **9xheap shim code**: Custom implementation for modern Windows compatibility

See individual source files for specific license information.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Test on multiple Windows versions
4. Submit a pull request

### Development Guidelines
- Maintain compatibility with Windows 95 through Windows 11
- Test with both Wine and native Windows
- Keep the core library independent of the shim
- Document any new hooking strategies

## Version History

### v2.0 (Current)
- Complete rewrite with modular architecture
- Dynamic instruction parsing for robust hooking
- Smart process heap detection with multiple fallbacks
- GCC MinGW build system
- Enhanced API resolution with multi-tier fallbacks

### v1.x (Legacy)
- MSVC-based build
- Static byte-count trampolines
- Brittle heap detection
- Monolithic architecture

---

*9xheap enables legacy Windows applications to run with proper heap behavior on modern systems, bridging decades of Windows evolution.*
