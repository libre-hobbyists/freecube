# Building FreeCube

> [!IMPORTANT]
> This is unfinished and could be updated at any moment, please check back frequently!

This assumes you already have an environment for C++ development. If you're on **Linux** you should have a C++ compiler preinstalled, if not you can install `g++` or `clang++` using your package manager. **Windows** users should have Visual Studio with the modules for **C++ Development** installed. **MacOS** users should have the `brew` utility installed and install cmake, Apple's provided `appleclang` compiler is sufficient for this project.

## Building on Windows (x86_64)

This project relies on CMake for building, it is required to build this. CMake comes with Visual Studio 2026/2024 if you select the **CMake** module on install.

Once the project is opened in Visual Studio (or your chosen IDE with cmake and a compiler ready), you can configure the project:

```sh
# You can choose your own -G generator! (cmake_policy flag is required!)
cmake -S . -B build -G Ninja <config flags> -DCMAKE_POLICY_VERSION_MINIMUM=3.5
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

Building on a MacOS system is straightforward. You'll need the `homebrew` utility installed alongside `cmake`. If you do not have Homebrew, you can install it from **[here](https://brew.sh/)** by following their insructions. Once brew is installed you'll want to install cmake and our recommended buildsystem, `ninja`

> [!TIP]
> MacOS comes with a clang-derived compiler called `AppleClang`, you do not need to install an extra compiler unless you want to!

```sh
brew install cmake
brew install ninja
```

Configure the project:
```sh
cmake -S . -B build -G Ninja <config flags> -DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

Build:
```sh
cd build
ninja
```

And in a few moments you'll have a ready-to-go arm64 copy of freecube!