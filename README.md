# ELTE GPU Cellular Automata

Cellular automata simulation

## Packages to install (Arch Linux)

```sh
gcc           # C++ compiler
opencl-clhpp  # OpenCL C++ headers
libglvnd      # OpenGL headers
libepoxy      # OpenGL extension loader
tinyxxd       # Kernel to header embedding
glfw          # Windowing
```

## Packages to install (Arch Linux, Windows cross compile)

```sh
mingw-w64-gcc               # C++ cross compiler
mingw-w64-opencl-headers    # OpenCL headers
mingw-w64-opencl-clhpp      # OpenCL C++ headers
mingw-w64-opencl-icd        # OpenCL ICD
mingw-w64-libepoxy          # OpenGL extension loader
mingw-w64-glfw              # Windowing
```

`mingw-w64-opencl-clhpp` is not in the AUR at time time of this commit, but it can be built from [this gist](https://gist.github.com/opekope2/8adf5b0f4ab339deacf214c6a63705e3).

## Setup

When using the clangd extension in VSCode, the `./build.sh gen_clangd` command can be used to generate a `.clangd` file, which helps with IntelliSense.

## Building (Linux)

Run `./build.sh` to build the executable. Run `./build.sh debug` to create a debug build. Run `OS=windows ./build.sh` to create a Windows build.

## Building (Windows)

`¯\_(ツ)_/¯`

## Running (Linux)

Run `bin/main`

## Running (Windows)

Run `bin\main.exe`

You'll need `libepoxy-0.dll`, `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, and `libwinpthread-1.dll` from the cross compiler toolchain.
