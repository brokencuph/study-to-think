# study-to-think
A student course grade assessment system.

# How to start development
First, clone this repository using
```
git clone --recurse-submodules <url>
```
If you have cloned it before without using `--recurse-submodules`, you should run the following commands to initiate submodules:
```
git submodule init
git submodule update
```

## Using Visual Studio (2019)
Just open the cloned project folder, and you will be able to build the project. Note that during CMake configuration, `vcpkg` will be bootstrapped and some dependencies (currently `sqlite` only) will be downloaded and installed.  
Sometimes network issues may block you from successfully downloading dependencies. In this case, view the build messages and resolve them accordingly.

## Using CMake command-line tool
Treat the project as a regular CMake project and type relevant commands. Commands may vary due to environment settings (such as build system and compiler). The following is for your reference:
```
cmake -S . -B build
cmake --build build
```