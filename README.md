# Nobody
 A N Body simulator written in c for simulating different systems of bodies using Barnes-Hut Algorithm.
 - Simulates around 2^13 bodies at 20fps.
 - Above was tested on 2 core cpu. No parallelism or gpu used.

 # Motivation
Lately I was interested in computer graphics and simulations so I decided to build this simple one for getting my feet wet in this domain.

## Setup

### Configuration

- Need to manually set the initial conditions in `body.c`,`body.h` and `utils.c` files
- You can simulate different system by using the `init_*` functions
- While running simulation use `tab` key toggle the drawing of Quadtree

### Pre-requisites

Before setting up the project, ensure you have the following tools installed:

- C Compiler (e.g., GCC)
- CMake (for building the project)
- Make (for building dependencies)
- MinGW (for Windows users)
- SDL2 (for graphics) (Linux and macOS only)

#### Windows

Scoop is a command-line installer for Windows that allows you to install software from the command line. It is recommended use it to install the required tools.

To install scoop, run the following command in PowerShell:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
Invoke-RestMethod -Uri https://get.scoop.sh | Invoke-Expression
```

After installing scoop, you can install the required tools by running the following commands:

```powershell
scoop bucket add main # Add the main bucket
scoop install main/gcc # Install the GCC compiler
scoop install main/make # Install the Make build tool
scoop install main/cmake # Install the CMake build tool
scoop install main/mingw # Install the MinGW compiler
```

If you want everything in one command, you can run the following command after adding the main bucket:

```powershell
scoop install main/gcc main/make main/cmake main/mingw
```

#### Linux

On Linux, you can install the required tools using the package manager of your distribution.

**Ubuntu**

```bash
# Common tools
sudo apt install gcc make cmake
# Raylib dependencies
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
# SDL2
sudo apt install libsdl2-dev
```

**Fedora**

```bash
# Common tools
sudo dnf install gcc make cmake
# Raylib dependencies
sudo dnf install alsa-lib-devel mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel libatomic
# SDL2
sudo dnf install SDL2-devel
```

**Arch**

```bash
# Common tools
sudo pacman -S gcc make cmake
# Raylib dependencies
sudo pacman -S alsa-lib mesa libx11 libxrandr libxi libxcursor libxinerama
# SDL2
sudo pacman -S sdl2
```

#### macOS

> NOTE: I don't have a macOS machine to test the instructions. If you encounter any issues, please let me know.

On macOS, you can install the required tools using Homebrew.

```bash
xcode-select --install
brew install gcc make cmake
brew install sdl2
```

### Building the project

Begin by [cloning the repository](https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository) to your local machine.


```bash
git clone https://github.com/raskunet/Nobody.git # Clone the repo
cd Nobody # Change directory to the project folder
```

#### Building Raylib

The project uses [Raylib](https://github.com/raysan5/raylib) for graphics. To build the project, you need to build the Raylib library first.

Navigate to the `external/raylib/src` directory and build the Raylib library according to your OS:

**Windows**

```powershell
cd external\raylib\src
mingw32-make PLATFORM=PLATFORM_DESKTOP
cd ..\..\.. # Go back to the project root
```

**Linux and macOS**

```bash 
cd external/raylib/src # From the project root
make PLATFORM=PLATFORM_DESKTOP
cd ../../.. # Go back to the project root
```

#### Generating the build files

After building the Raylib library, you can generate the build files using CMake.

**Linux and macOS**

```bash
# From the project root
mkdir build # Create a build directory
cd build
cmake ..
make
```

**Windows**

```powershell
# From the project root
mkdir build # Create a build directory
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

### Running the project

After building the project, you can run the executable generated in the `build` directory.

**Linux and macOS**

```bash
# From the build directory
cd build
./Nobody
```

**Windows**

```powershell
# From the build directory
cd build
./Nobody.exe
```


