cmake_minimum_required(VERSION 3.15)
project(template)

set(CMAKE_C_STANDARD 11)

# setting parameters for raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/raylib)

add_compile_options("-Wall" "-Wextra" "-ggdb" "-static" "-static-libgcc")

if(EMSCRIPTEN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY --shell-file ../vendor/raylib/src/minshell.html")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	set(BUILD_WEB_SHELL vendor/raylib/src/minshell.html)
endif()

file(GLOB_RECURSE SRC
	src/*.c
	vendor/clogs/src/*.c
	vendor/raylibExtras/rlExtrasC/*.c
)

add_executable(game ${SRC})
target_include_directories(game
	PUBLIC src
	PUBLIC vendor/vector/src
	PUBLIC vendor/clogs/src
	PUBLIC vendor/raylib/external
	PUBLIC vendor/raylibExtras/rlExtrasC
)

target_link_libraries(game PRIVATE raylib)
