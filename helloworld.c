#include <stdio.h>
#define LUAJIT_USE_IO_FUNCTIONS 1
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <luajit.h>

static FILE* s_fopen(const char* filename, const char* mode) {
#ifdef _WIN32
    FILE* ret;
    fopen_s(&ret, filename, mode);
    return ret;
#else
	return fopen(filename, mode);
#endif
}
int main(void) {
    lua_State* L = luaL_newstate();  // Create a new Lua state
    if (L == NULL) {
        fprintf(stderr, "Failed to create Lua state\n");
        return 1;
    }


    luaL_openlibs(L);  // Open standard libraries
    luaL_registerFileFunctions((lua_io_functions) { // Set file functions
        .fopen = s_fopen,
        .fclose = fclose,
        .fread = fread,
        .fwrite = fwrite,
        .feof = feof,
        .ferror = ferror,
		.ftell = ftell,
		.fseek = fseek
	});

	
    // Execute a simple Lua script

    if (luaL_dostring(L, "print('Hello, World!')")) {
        fprintf(stderr, "Failed to execute Lua script: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return 1;
    }

    // 1. Check current JIT status
    int status = luaJIT_setmode(L, -1, LUAJIT_MODE_ENGINE | LUAJIT_MODE_OFF);
    printf("JIT was %s\n", (status == 0) ? "already OFF" : "ON (now turned OFF)");

    // 2. Disable JIT compilation globally
    luaJIT_setmode(L, -1, LUAJIT_MODE_ENGINE | LUAJIT_MODE_OFF);

    // 3. Run Lua code to test JIT (no compilation will occur)
    const char* lua_code =
        "local s = 0\n"
        "for i=1,1000000 do s = s + i end\n"
        "print('Sum (JIT OFF):', s)";

    if (luaL_dostring(L, lua_code)) {
        fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
    }

    // 4. Re-enable JIT
    luaJIT_setmode(L, -1, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON);
    printf("JIT re-enabled.\n");

    // 5. Run the same code with JIT active
    if (luaL_dostring(L, lua_code)) {
        fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
    }


    lua_close(L);  // Close the Lua state
    return 0;
}
