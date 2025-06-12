cmake_minimum_required(VERSION 3.15)
project(HelloWorld)

# Set the source file
set(SOURCE_FILE helloworld.c)

# Add the executable
add_executable(helloworld ${SOURCE_FILE})

# Specify the path to LuaJIT library
set(LUAJIT_PATH ${CMAKE_SOURCE_DIR}/src)

# Add LuaJIT as a subdirectory
add_subdirectory(${LUAJIT_PATH} LuaJIT_Build)

# Link LuaJIT library
target_link_libraries(helloworld libluajit)

# Add include directory
target_include_directories(helloworld PRIVATE ${LUAJIT_PATH})


# Copy lua51.dll to the output directory
add_custom_command(TARGET helloworld POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libluajit> $<TARGET_FILE_DIR:helloworld>)
