# Building FreeCube

> [!IMPORTANT]
> This is unfinished and could be updated at any moment, please check back frequently!

This assumes you already have an environment for C++ development. If you're on **Linux** you should have a C++ compiler preinstalled, if not you can install `g++` or `clang++` using your package manager. **Windows** users should have Visual Studio with the modules for **C++ Development** installed. **MacOS** users should have the `brew` utility installed and install cmake, Apple's provided `appleclang` compiler is sufficient for this project.

## Building on Windows (x86_64)

This project relies on CMake for building, it is required to build this. CMake comes with Visual Studio 2026/2024 if you select the **CMake** module on install.

Once the project is opened in Visual Studio (or your chosen IDE with cmake and a compiler ready), you can configure the project:

```sh
# You can choose your own -G generator! (cmake_policy flag is required!)
cmake -S . -B build -G Ninja <custom flags> -DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

> [!NOTE]
> On Visual Studio, it may automatically configure the project with all the default settings.

Building is as easy as running a couple commands, or a single keystroke in some IDEs:
```sh
cd build
ninja
# Or your chosen build system!
```

## Building on Linux (x86_64)

## Building on MacOS (arm64)