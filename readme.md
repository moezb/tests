This is an incomplete work (that you can build and can run it have some bugs due to the incompleteness, the idea is to use a sparse memory mapped file for all possible unsigned int key hashes (the dictionary) to store key + fix sized entry (the tlv of the key value ) avoid search and sorting operation while building the dictionary, using sparse file will help make insertion faster (managed by the os) and disk usage optimized. 

## How to build:
1) Install last version of conan client  to install apr/1.7.0 dependency : 

`` sudo apt install conan
``

2) Install last version of Ninja  build system

`` sudo apt install ninja-build
``

3) We de need cmake for debian , the apt installed version cmake is 3.18

`` cmake -G "Ninja" -DCMAKE_BUILD_TYPE:STRING="Debug" -B./build
`` 

`` cmake --build ./build --config Debug
`` 

if you have a CMake version 3.20 or later you can use the config preset *linux-debug* located in [./CMakePresets.json](./CMakePresets.json)  ( you need to build cmake from the source)

`` cmake --preset "linux-debug" 
`` 

`` cmake --build --preset "buildPresets"
`` 

 you can list all presets with:

 `` cmake --list-presets=all .
 `` 
