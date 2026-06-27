# ELTE GPU Cellular Automata

Cellular automata simulation

## Dependencies

### Arch Linux

#### Make

```sh
gcc                         # C++ compiler
opencl-headers              # OpenCL headers
opencl-clhpp                # OpenCL C++ headers
tinyxxd                     # Kernel to header embedding
```

#### GUI

```sh
libglvnd                    # OpenGL headers
libepoxy                    # OpenGL extension loader
glfw                        # Windowing
```

#### Windows cross compilation

```sh
mingw-w64-gcc               # C++ cross compiler
mingw-w64-opencl-headers    # OpenCL headers
mingw-w64-opencl-clhpp      # OpenCL C++ headers
mingw-w64-opencl-icd        # OpenCL ICD
mingw-w64-libepoxy          # OpenGL extension loader
mingw-w64-glfw              # Windowing
```

`mingw-w64-opencl-clhpp` is not in the AUR at time time of this commit, but it can be built from [this gist](https://gist.github.com/opekope2/8adf5b0f4ab339deacf214c6a63705e3).

### Termux

#### Prerequisites

```sh
git                         # For compiling tinyxxd
make                        # For compiling tinyxxd
```

#### Make

```sh
clang                       # C++ compiler
opencl-headers              # OpenCL headers
opencl-clhpp                # OpenCL C++ headers
ocl-icd                     # OpenCL ICD
```

#### Runtime

```sh
clvk                        # OpenCL
vulkan-loader-android       # DO NOT USE vulkan-loader-generic, otherwise the benchmark will run on the CPU (llvmpipe)
```

## Setup

When using the clangd extension in VSCode, the `./build.sh gen_clangd` command can be used to generate a `.clangd` file, which helps with IntelliSense.

## Compiling

### Linux

Run `./build.sh` to build the executable. Run `./build.sh debug` to create a debug build. Run `OS=windows ./build.sh` to create a Windows build.

### Termux

`tinyxxd` is not packaged in Termux as of this commit, so you'll need to compile it yourself.

```sh
# 1. Clone tinyxxd
git clone https://github.com/xyproto/tinyxxd.git

# 2. Build tinyxxd
(cd tinyxxd && make)

# 3. Add tinyxxd to path
export PATH=$PATH:$PWD/tinyxxd

# 4. Clone project
git clone https://github.com/opekope2/ElteGpuCellularAutomata.git

# 5. Build project
cd ElteGpuCellularAutomata
OS=android GUI=0 ./build.sh
```

### Windows

`¯\_(ツ)_/¯`

## Running

### Linux

Run `bin/cellular_automata`

### Termux

You can only run benchmarks in Termux. Run `bin/cellular_automata`

### Windows

Run `bin\cellular_automata.exe`

You'll need `libepoxy-0.dll`, `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, and `libwinpthread-1.dll` from the cross compiler toolchain.

## Usage

* ⬅️: Pan left by 1 cell
* ➡️: Pan right by 1 cell
* ⬆️: Pan up by 1 cell
* ⬇️: Pan down by 1 cell
* `-`: Decrease zoom by 1 step
* `=`: Increase zoom by 1 step
* `C`: Simulate Conway's Game of Life
* `T`: Simulate Turmite
* `SPACE`: Pause/resume simulation
* `[`: Decrease simulation speed by 1
* `]`: Increase simulation speed by 1
* `.`: Step the simulation 1 step forward
* `CTRL`+`V`: Load data from clipboard
* `CTRL`+`B`: Load rule from clipboard
* `Q`: Quit

Hold `CTRL` to increase or decrease values by 10.
Hold `SHIFT` to increase or decrease values by 100.
Hold `CTRL` and `SHIFT` to increase or decrease values by 1000.

Specify the `PLATFORM` and `DEVICE` environment variables to choose OpenCL platform and device instead of the default one.
Specify the `PLATFORM=list` and `DEVICE=list` environment variables to print the available OpenCL platforms and devices.

Specify the `SIZE` environment variable to override the simulation width and height (default: 4096). The sumulation grid is `SIZE`x`SIZE`, and warps around like a torus.

## Data

### Conway's Game of Life

[Run Length Encoded](https://conwaylife.com/wiki/Run_Length_Encoded) data without "# lines" or the header line.
Loading data with "# lines" or the header line may lead to corrupted state.
Data without an `!` will fail to load.

### Turmites

The Turmite simulation doesn't support loading data.

## Rules

### Conway's Game of Life

A [Rulestring](https://conwaylife.com/wiki/Rulestring#Birth/survival_notation) in Birth/survival notation.

### Turmites

This program recognizes the following syntax for a Turmite rule string:

#### Rule

`{state_0, state_1, ...}`

You must specify at least 1 state, up to 256 states, indexed from 0.

#### State

`{color_0, color_1, ...}`

You must specify at least 1 color, up to 128 colors, indexed from 0.
All states must have exactly the same number of colors.

#### Color

`{new_color, turn, next_state}`

* `new_color`: The color index to write to the Turmite's cell
* `turn`
  * `1`: Don't turn
  * `2`: Turn right
  * `4`: U-turn
  * `8`: Turn left
* `next_state`: The state index to transition to

## Benchmark

Specify the `BENCHMARK` environment variable to run a specific benchmark.
Specify `BENCHMARK=list` to show a list of all benchmarks.

### Supported environment variables

* `SIZE`: The simulation (square) size to benchmark
* `BENCHMARK_GENERATIONS`: The number generations to run the simulation for
* `BENCHMARK_DATA`: The generation 0
