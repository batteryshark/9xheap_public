# 9xheap - Windows 95-XP Heap Emulation for Modern Systems
# Unified build: core static library + shim DLL
#
# Architecture:
#   core/     - Microsoft's emulated heap code (static library)
#   shim/     - Hooking and patching code (DLL)
#
# Cross-compile from WSL/Linux using: make
# Requires: i686-w64-mingw32-gcc

# Directories
BUILD_DIR = build
DIST_DIR = dist
CORE_DIR = core
SHIM_DIR = shim

# Output files
TARGET_DLL = $(DIST_DIR)/9xheap.dll
CORE_LIB = $(BUILD_DIR)/lib9xheap_core.a
IMPLIB = $(BUILD_DIR)/lib9xheap.dll.a

# Source files
CORE_SOURCES = $(CORE_DIR)/9xheap_core.c $(CORE_DIR)/emulateheap_heap.c $(CORE_DIR)/emulateheap_lmem.c $(CORE_DIR)/emulateheap_support.c
SHIM_SOURCES = $(SHIM_DIR)/shim_minimal.c $(SHIM_DIR)/binding_helpers.c $(SHIM_DIR)/bindings_kernel32.c $(SHIM_DIR)/bindings_ntdll.c $(SHIM_DIR)/mem.c $(SHIM_DIR)/shim_entry.c

# Object files
CORE_OBJECTS = $(CORE_SOURCES:$(CORE_DIR)/%.c=$(BUILD_DIR)/core_%.o)
SHIM_OBJECTS = $(SHIM_SOURCES:$(SHIM_DIR)/%.c=$(BUILD_DIR)/shim_%.o)

# Compiler and tools
CC = i686-w64-mingw32-gcc
AR = i686-w64-mingw32-ar
STRIP = i686-w64-mingw32-strip

# Base compiler flags
BASE_CFLAGS = -Wall -Wno-unknown-pragmas -Wno-discarded-qualifiers -Wno-incompatible-pointer-types
BASE_CFLAGS += -O2 -m32 -march=i386 -mtune=i386
BASE_CFLAGS += -DWIN32 -DNDEBUG -D_CONSOLE
BASE_CFLAGS += -fno-stack-protector -fno-stack-check

# Core library flags (static library, no CRT)
CORE_CFLAGS = $(BASE_CFLAGS) -DBUILDING_9XHEAP_CORE -nostdlib -fno-builtin -I$(CORE_DIR)

# Shim DLL flags (includes core headers)
SHIM_CFLAGS = $(BASE_CFLAGS) -I$(CORE_DIR) -I$(SHIM_DIR)

# Linker flags for DLL
LDFLAGS = -shared -m32
LDFLAGS += -Wl,--subsystem,console
LDFLAGS += -Wl,--enable-stdcall-fixup
LDFLAGS += -Wl,--kill-at
LDFLAGS += -Wl,--entry,_DllMainCRTStartup@12
LDFLAGS += -static-libgcc
LDFLAGS += -Wl,--out-implib,$(IMPLIB)

# System libraries
LIBS = -lkernel32 -lntdll

.PHONY: all clean dirs test info

all: $(TARGET_DLL)

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(DIST_DIR)

# Build core static library
$(CORE_LIB): $(CORE_OBJECTS) | dirs
	$(AR) rcs $@ $(CORE_OBJECTS)
	@echo "Built core static library: $@"

# Build final DLL
$(TARGET_DLL): $(SHIM_OBJECTS) $(CORE_LIB) 9xheap_shim.def | dirs
	$(CC) $(LDFLAGS) -o $@ $(SHIM_OBJECTS) $(CORE_LIB) $(LIBS) 9xheap_shim.def
	$(STRIP) --strip-unneeded $@
	@echo "Built: $@"
	@echo "Import library: $(IMPLIB)"

# Core object files (static library, no CRT)
$(BUILD_DIR)/core_%.o: $(CORE_DIR)/%.c | dirs
	$(CC) $(CORE_CFLAGS) -c $< -o $@

# Shim object files (DLL)
$(BUILD_DIR)/shim_%.o: $(SHIM_DIR)/%.c | dirs
	$(CC) $(SHIM_CFLAGS) -c $< -o $@

# Test target
test: $(TARGET_DLL)
	cp $(TARGET_DLL) .
	./test_static.exe

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DIST_DIR)
	rm -f 9xheap.dll
	@echo "Cleaned build directories"

# Show configuration
info:
	@echo "9xheap Build Configuration:"
	@echo "  Architecture: core/ (static lib) + shim/ (DLL)"
	@echo "  Target DLL: $(TARGET_DLL)"
	@echo "  Core static lib: $(CORE_LIB)"
	@echo "  Core sources: $(CORE_SOURCES)"
	@echo "  Shim sources: $(SHIM_SOURCES)"
	@echo "  Compiler: $(CC)"
