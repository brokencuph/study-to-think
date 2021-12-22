# FEIRAN (original study-to-think)
A student course grade assessment system written in C++ and Qt.

Prebuilt Windows binary available [here](https://github.com/brokencuph/study-to-think/releases/tag/v1.0beta).

## How to build/develop
> **TL;DR**: clone the repository with submodules, install Qt6 and set up env vars, and build it normally like any CMake project.

First, clone this repository using
```bash
git clone --recurse-submodules <url>
```
If you have cloned it before without using `--recurse-submodules`, you should run the following commands to initiate submodules:
```bash
git submodule init
git submodule update
```
Then follow instructions below to set up your environment.

### Install Qt
Currently this project targets Qt6, and is tested to build with Qt 6.2.1. This is not included in the `vcpkg.json` file because it is too large and time-consuming to be installed by vcpkg. If you want to have it automatically installed and configured, try adding `qt` and `qtcharts` as dependencies to `vcpkg.json` file (not tested).

After manually downloading and installing Qt (also QtCharts), you should set environment variable `CMAKE_PREFIX_PATH` to your installation path (e.g. `C:\Qt\6.2.1\msvc2019_64`) in order to let CMake find Qt library files. To run the built binary, you should also make sure that the Qt library files can be found by the OS (e.g. by setting `PATH` in Windows or `LD_LIBRARY_PATH` in Linux).

### Other dependencies
Other dependencies are managed by `vcpkg`, which should be automatically triggered during CMake process, so normally you need not do anything. If something fails, try to debug by inspecting CMake logs.

## CMake references
### Using Visual Studio (2019)
Just open the cloned project folder, and you will be able to build the project. Note that during CMake configuration, `vcpkg` will be bootstrapped and some dependencies will be downloaded and installed.  
Sometimes network issues may block you from successfully downloading dependencies. In this case, view the build messages and resolve them accordingly.

### Using CMake command-line tool
Treat the project as a regular CMake project and type relevant commands. Commands may vary due to environment settings (such as build system and compiler). The following is for your reference:
```
cmake -S . -B build
cmake --build build
```