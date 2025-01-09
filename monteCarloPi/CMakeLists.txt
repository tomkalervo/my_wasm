cmake_minimum_required(VERSION 3.14)
project(MonteCarlo_Pi)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL2_INCLUDE_DIR "/usr/local/include/SDL2")
set(SDL2_LIBRARY "/usr/local/lib/libSDL2.dylib")
set(TTF_LIBRARY "/usr/local/lib/libSDL2_ttf.dylib")

include_directories(${SDL2_INCLUDE_DIR})
add_executable(mcp mcp.cpp)

if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s USE_SDL_TTF=2 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # Generate an HTML shell for WASM
    target_link_libraries(mcp "-s USE_SDL=2 -s USE_SDL_TTF=2")
else()
    target_link_libraries(mcp ${SDL2_LIBRARY} ${TTF_LIBRARY})
endif()
