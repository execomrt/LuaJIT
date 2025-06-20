/*
** Standard library header.
** Copyright (C) 2005-2025 Mike Pall. See Copyright Notice in luajit.h
*/

#ifndef _LUALIB_H
#define _LUALIB_H

#include "lua.h"

#define LUA_FILEHANDLE	"FILE*"

#define LUA_COLIBNAME	"coroutine"
#define LUA_MATHLIBNAME	"math"
#define LUA_STRLIBNAME	"string"
#define LUA_TABLIBNAME	"table"
#define LUA_IOLIBNAME	"io"
#define LUA_OSLIBNAME	"os"
#define LUA_LOADLIBNAME	"package"
#define LUA_DBLIBNAME	"debug"
#define LUA_BITLIBNAME	"bit"
#define LUA_JITLIBNAME	"jit"
#define LUA_FFILIBNAME	"ffi"


LUALIB_API int luaopen_base(lua_State *L);
LUALIB_API int luaopen_math(lua_State *L);
LUALIB_API int luaopen_string(lua_State *L);
LUALIB_API int luaopen_table(lua_State *L);
LUALIB_API int luaopen_io(lua_State *L);
LUALIB_API int luaopen_os(lua_State *L);
LUALIB_API int luaopen_package(lua_State *L);
LUALIB_API int luaopen_debug(lua_State *L);
LUALIB_API int luaopen_bit(lua_State *L);
LUALIB_API int luaopen_jit(lua_State *L);
LUALIB_API int luaopen_ffi(lua_State *L);
LUALIB_API int luaopen_string_buffer(lua_State *L);

LUALIB_API void luaL_openlibs(lua_State *L);

#ifndef lua_assert
#define lua_assert(x)	((void)0)
#endif

#ifdef LUAJIT_USE_IO_FUNCTIONS

#include <stdio.h>
typedef struct lua_Io_functions {
	FILE* (*fopen)(const char* filename, const char* mode);
	int (*fclose)(FILE* fp);
	size_t(*fread)(void* ptr, size_t size, size_t count, FILE* fp);
	size_t(*fwrite)(const void* ptr, size_t size, size_t count, FILE* fp);
	int (*feof)(FILE* fp);
	int (*ferror)(FILE* fp);
	int(*fseek)(FILE* fp, long offset, int whence);
	long (*ftell)(FILE* fp);
} lua_io_functions;

LUALIB_API void luaL_registerFileFunctions(lua_io_functions funcs);

typedef struct lua_Mem_functions {
	void* (*malloc)(size_t length);
	void* (*realloc)(void* ptr, size_t length);
	void (*free)(void* ptr);
} lua_mem_functions;

LUALIB_API void luaL_registerMemFunctions(lua_mem_functions funcs);

typedef struct lua_Print_functions {
	void (*print)(const char* text);
} lua_print_functions;

LUALIB_API void luaL_registerPrintFunctions(lua_print_functions funcs);

LUALIB_API lua_io_functions* luaL_getFileFunctions(void);

#define l_fopen luaL_getFileFunctions()->fopen
#define l_fclose luaL_getFileFunctions()->fclose
#define l_fread luaL_getFileFunctions()->fread
#define l_fwrite luaL_getFileFunctions()->fwrite
#define l_feof luaL_getFileFunctions()->feof
#define l_ferror luaL_getFileFunctions()->ferror
#define l_fseek luaL_getFileFunctions()->fseek
#define l_ftell luaL_getFileFunctions()->ftell

// A zero value indicates success.
static inline int l_fflush(FILE* fp)
{
	(void)fp;
	return 0;
}
// This function returns the character read as an unsigned char cast to an int or EOF on end of file or error.
static inline int l_fgetc(FILE* fp)
{
	unsigned char byte;
	if (l_fread(&byte, 1, 1, fp) == 0)
	{
		return -1;
	}
	return byte;
}
static inline void l_ungetc(int c, FILE* fp)
{
	if (c == -1) return;
	l_fseek(fp, -1, SEEK_CUR);
}

static inline char* l_fgets(char* _Buffer, int _MaxCount, FILE* _Stream)
{
	// implement using lua_io_funcs.fread
	size_t bytesRead = l_fread(_Buffer, 1, _MaxCount - 1, _Stream);
	if (bytesRead > 0) {
		_Buffer[bytesRead] = '\0'; // Null-terminate the string
		return _Buffer;
	}
	if (l_ferror(_Stream)) {
		return NULL; // Error occurred
	}
	return NULL; // EOF reached
}

#endif // LUAJIT_USE_IO_FUNCTIONS

#endif
