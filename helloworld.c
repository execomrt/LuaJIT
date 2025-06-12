#include <stdio.h>
#define LUAJIT_USE_IO_FUNCTIONS 1
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int main(void) {
    lua_State* L = luaL_newstate();  // Create a new Lua state
    if (L == NULL) {
        fprintf(stderr, "Failed to create Lua state\n");
        return 1;
    }

    luaL_openlibs(L);  // Open standard libraries
    luaL_registerFileFunctions((lua_io_functions) { // Set file functions
        .fopen = fopen,
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

    lua_close(L);  // Close the Lua state
    return 0;
}
