cmake_minimum_required(VERSION 3.15)
project(LuaJIT)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LUA_OUTPUT_LIB "libluajit")

if(APPLE)
    message(STATUS "Building for iOS")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch armv7 -miphoneos-version-min=9.0")    
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS}")
    set(OBJ_EXT "o")
    set(LIB_EXT "a")
    set(ASM_EXT "s")

elseif(ANDROID)
    message(STATUS "Building for Android")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfloat-abi=softfp -mfpu=neon")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -march=armv7-a -mfloat-abi=softfp -mfpu=neon")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS}")
    set(OBJ_EXT "o")
    set(LIB_EXT "a")
    set(ASM_EXT "s")

else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /nologo /W3 /D_CRT_SECURE_NO_DEPRECATE /D\"_CRT_STDIO_INLINE=__declspec(dllexport)__inline\"")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /INCREMENTAL:NO")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /INCREMENTAL:NO")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi")
    set(OBJ_EXT "obj")
    set(LIB_EXT "lib")
    set(ASM_EXT "asm")
endif()



option(LUA_BUILD_AS_DLL "Build as shared library" ON)
option(LUA_USE_APICHECK "Turn on assertions for the Lua/C API to debug problems with lua_* calls. This is rather slow, use only while developing C libraries/embeddings." OFF)
option(LUA_USE_ASSERT "Turn on assertions for the whole LuaJIT VM. This significantly slows down everything. Use only if you suspect a problem with LuaJIT itself." OFF)
option(LUAJIT_DISABLE_DEBUGINFO "Disable debug information" OFF)
option(LUAJIT_DISABLE_FFI "Permanently disable the FFI extension to reduce the size of the LuaJIT executable. But please consider that the FFI library is compiled-in, but NOT loaded by default. It only allocates any memory, if you actually make use of it." ON)
option(LUAJIT_DISABLE_GC64 "Disable 64-bit garbage collector" OFF)
option(LUAJIT_DISABLE_JIT "Disable the JIT compiler, i.e. turn LuaJIT into a pure interpreter." ON)
option(LUAJIT_DISABLE_JITUTIL "Disable JIT utility functions" OFF)
option(LUAJIT_DISABLE_TAILCALL "Disable tail call optimization" OFF)
option(LUAJIT_DISABLE_VMEVENT "Disable VM event support" OFF)
option(LUAJIT_ENABLE_LUA52COMPAT "Features from Lua 5.2 that are unlikely to break existing code are enabled by default. Some other features that *might* break some existing code (e.g. __pairs or os.execute() return values) can be enabled here. Note: this does not provide full compatibility with Lua 5.2 at this time." OFF)
option(LUAJIT_USE_GDBJIT "This is the client for the GDB JIT API. GDB 7.0 or higher is required to make use of it. See lj_gdbjit.c for details. Enabling this causes a non-negligible overhead, even when not running under GDB." OFF)
option(LUAJIT_USE_IO_FUNCTIONS "Use I/O functions with luaL_registerFileFunctions" ON)
option(LUAJIT_USE_PERFTOOLS "Use Google Performance Tools" OFF)
option(LUAJIT_USE_SYSMALLOC "Use the system provided memory allocator (realloc) instead of the bundled memory allocator. This is slower, but sometimes helpful for debugging. It's helpful for Valgrind's memcheck tool, too. This option cannot be enabled on x64, since the built-in allocator is mandatory." OFF)
option(LUAJIT_USE_VALGRIND "This option is required to run LuaJIT under Valgrind. The Valgrind header files must be installed. You should enable debug information, too." OFF)


option(LJ_TARGET_CONSOLE "Disable GetEnv" ON)
option(LJ_TARGET_XBOX360 "Xbox 360" OFF)
option(LJ_TARGET_XBOXONE "Xbox One" OFF)

# Add definitions based on options
if(LUA_BUILD_AS_DLL)
    add_definitions(-DLUA_BUILD_AS_DLL)
endif()
if(LUAJIT_DISABLE_GC64)
    add_definitions(-DLUAJIT_DISABLE_GC64)
endif()
if(LUAJIT_DISABLE_VMEVENT)
    add_definitions(-DLUAJIT_DISABLE_VMEVENT)
endif()
if(LUAJIT_DISABLE_JIT)
    add_definitions(-DLUAJIT_DISABLE_JIT)
endif()
if(LUAJIT_DISABLE_JITUTIL)
    add_definitions(-DLUAJIT_DISABLE_JITUTIL)
endif()
if(LUAJIT_DISABLE_DEBUGINFO)
    add_definitions(-DLUAJIT_DISABLE_DEBUGINFO)
endif()
if(LUAJIT_DISABLE_TAILCALL)
    add_definitions(-DLUAJIT_DISABLE_TAILCALL)
endif()
if(LUAJIT_DISABLE_FFI)
    add_definitions(-DLUAJIT_DISABLE_FFI)
endif()
if(LUAJIT_USE_IO_FUNCTIONS)
    add_definitions(-DLUAJIT_USE_IO_FUNCTIONS)
endif()
if(LUAJIT_ENABLE_LUA52COMPAT)
    add_definitions(-DLUAJIT_ENABLE_LUA52COMPAT)
endif()
if(LJ_TARGET_CONSOLE)
    add_definitions(-DLJ_TARGET_CONSOLE)
endif()


# Define LuaJIT version (if available)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../.git")
  execute_process(COMMAND git show -s --format=%ct
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.."
    OUTPUT_VARIABLE GIT_COMMIT_TIME
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/luajit_relver.txt" "${GIT_COMMIT_TIME}")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../.relver")
  # file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../.relver" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/luajit_relver.txt")
endif()

# Add dynasm directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dynasm)

# lua_host_minilua
add_executable(lua_host_minilua host/minilua.c)

# Custom command to generate host/buildvm_arch.h
set(BUILDVM_ARCH_H "${CMAKE_CURRENT_SOURCE_DIR}/host/buildvm_arch.h")


if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(DASC "vm_arm64.dasc")
    message(STATUS "Using ARM64 dynasm file")
else()
    set(DASC "vm_x64.dasc") # Default to x64
    message(STATUS "Using X64 dynasm file")
endif()

set(DASM "${CMAKE_CURRENT_SOURCE_DIR}/../dynasm/dynasm.lua")
if (WIN32)
    set(DASMFLAGS
        ${CMAKE_CURRENT_SOURCE_DIR}/../dynasm/dynasm.lua
        -LN
        -D WIN
        -D FPU
        -D P64
        -D ENDIAN_LE
        -o
        ${BUILDVM_ARCH_H}
        ${DASC}
    )
else()
    set(DASMFLAGS
        ${CMAKE_CURRENT_SOURCE_DIR}/../dynasm/dynasm.lua
        -LN
        -D NO_UNWIND
        -D P64
        -o
        ${BUILDVM_ARCH_H}
        ${DASC}
    )
endif()


add_custom_command(
  OUTPUT ${BUILDVM_ARCH_H}
  COMMAND $<TARGET_FILE:lua_host_minilua> ${DASMFLAGS}
  DEPENDS lua_host_minilua ${CMAKE_CURRENT_SOURCE_DIR}/../dynasm/dynasm.lua ${CMAKE_CURRENT_SOURCE_DIR}/${DASC}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Generating host/buildvm_arch.h"
)
# Custom command to generate version file
set(LJ_VERSION_H "${CMAKE_CURRENT_SOURCE_DIR}/luajit.h")
add_custom_command(
  OUTPUT ${LJ_VERSION_H}
  COMMAND $<TARGET_FILE:lua_host_minilua> ${CMAKE_CURRENT_SOURCE_DIR}/host/genversion.lua
  DEPENDS $<TARGET_FILE:lua_host_minilua> ${CMAKE_CURRENT_SOURCE_DIR}/host/genversion.lua ${CMAKE_CURRENT_SOURCE_DIR}/luajit_relver.txt
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} # Changed working directory
  COMMENT "Generating luajit.h"
)

# lua_host_buildvm
file(GLOB BUILDVM_SOURCES host/buildvm*.c)
add_executable(lua_host_buildvm ${BUILDVM_SOURCES})
target_include_directories(lua_host_buildvm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../dynasm)

# Add the generated headers to the buildvm target's source list
add_library(gen_headers INTERFACE)
add_custom_target(lua_generate_headers ALL DEPENDS ${BUILDVM_ARCH_H} ${LJ_VERSION_H})
add_dependencies(lua_host_buildvm lua_generate_headers)

# Cross compilation setup

# List of library source files
set(LIB_SOURCES
  lib_base.c
  lib_math.c
  lib_bit.c
  lib_string.c
  lib_table.c
  lib_init.c
  lib_aux.c
  lib_io.c
  lib_os.c
  lib_package.c
  lib_debug.c
  lib_jit.c
  lib_buffer.c
)

# Custom command to generate object file and headers
add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_vm.obj
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_bcdef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_ffdef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_libdef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_recdef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/jit/vmdef.lua
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_folddef.h
  COMMAND
    $<TARGET_FILE:lua_host_buildvm> -m peobj -o ${CMAKE_CURRENT_SOURCE_DIR}/lj_vm.${OBJ_EXT} &&
    $<TARGET_FILE:lua_host_buildvm> -m bcdef -o ${CMAKE_CURRENT_SOURCE_DIR}/lj_bcdef.h ${LIB_SOURCES} &&
    $<TARGET_FILE:lua_host_buildvm> -m ffdef -o ${CMAKE_CURRENT_SOURCE_DIR}/lj_ffdef.h ${LIB_SOURCES} &&
    $<TARGET_FILE:lua_host_buildvm> -m libdef -o ${CMAKE_CURRENT_SOURCE_DIR}/lj_libdef.h ${LIB_SOURCES} &&
    $<TARGET_FILE:lua_host_buildvm> -m recdef -o ${CMAKE_CURRENT_SOURCE_DIR}/lj_recdef.h ${LIB_SOURCES} &&
    $<TARGET_FILE:lua_host_buildvm> -m vmdef -o ${CMAKE_CURRENT_SOURCE_DIR}/jit/vmdef.lua ${LIB_SOURCES} &&
    $<TARGET_FILE:lua_host_buildvm> -m folddef -o ${CMAKE_CURRENT_SOURCE_DIR}/lj_folddef.h lj_opt_fold.c
  DEPENDS lua_host_buildvm ${LIB_SOURCES} lj_opt_fold.c
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Generating object file and headers with lua_host_buildvm"
)

# Create a custom target for generating headers
add_custom_target(lua_generate_vm_headers
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_vm.obj
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_bcdef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_ffdef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_libdef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_recdef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/jit/vmdef.lua
    ${CMAKE_CURRENT_SOURCE_DIR}/lj_folddef.h
)

# LuaJIT shared library
file(GLOB LUAJIT_SOURCES lj_*.c)
list(APPEND LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lj_vm.obj)
add_library(${LUA_OUTPUT_LIB} SHARED ${LUAJIT_SOURCES} ${LIB_SOURCES})
target_include_directories(${LUA_OUTPUT_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(${LUA_OUTPUT_LIB} lua_generate_vm_headers)

# luajit executable
add_executable(luajit luajit.c)
target_link_libraries(luajit ${LUA_OUTPUT_LIB})
target_include_directories(luajit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(luajit lua_generate_vm_headers)

