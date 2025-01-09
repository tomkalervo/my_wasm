# Monte Carlo Pi Visualizer

A program built with **SDL2** that visualizes how Pi can be approximated using the Monte Carlo method. It supports both native compilation and WebAssembly (`.wasm`) via **Emscripten**, allowing you to run it on the web.

## Features
- Visual representation of Monte Carlo simulations to approximate Pi.
- Native builds with **CMake** for desktops.
- WebAssembly build for web compatibility using **Emscripten**.
- Uses **SDL2** and **SDL_ttf** for graphics and text rendering.

---

## Build Instructions

### Prerequisites
- **SDL2** and **SDL_ttf** installed for native builds.
- **CMake** installed for native compilation.
- **Emscripten SDK** installed for WebAssembly builds (see [Emscripten setup](https://emscripten.org/docs/getting_started/downloads.html)).

### Native Build (CMake)
1. Configure the build:
   ```bash
   cmake -S . -B build
    ```
2. Build the project:
   ```bash 
   cmake --build build 
   ```
3. Run the executable from the **build** directory

### WebAssembly Build(Emscripten) ###
1. Compile the program with Emscripten:
   ```bash
   emcc mcp.cpp -o mcp.html -s USE_SDL=2 -s USE_SDL_TTF=2 -s ALLOW_MEMORY_GROWTH=1 --preload-file fonts
   ```
- **-s USE_SDL=2**: Enables SDL2.
- **-s USE_SDL_TTF=2**: Enables SDL_ttf for font rendering.
- **--preload-file fonts**: Preloads the fonts directory for use by the program.
- **-s ALLOW_MEMORY_GROWTH=1**: Allows memory to grow dynamically.
   
 2.  Open the generated **mcp.html** in a browser to view the simulation.
