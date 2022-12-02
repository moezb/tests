cmake_minimum_required(VERSION 3.18)
project(TestJsonPack VERSION 0.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
include(conan)
set(LANGUAGE c)
set(CMAKE_C_STANDARD 11)

set(CI_INSTALL_DIR "${CMAKE_BINARY_DIR}/conan")
set(CI_BUILD_TYPE "build_type=Debug")
if (CMAKE_BUILD_TYPE STREQUAL "linux-release")
                # conan-center 
                set(CI_BUILD_TYPE "build_type=Release")
endif()
 conan_cmake_install(
            PATH_OR_REFERENCE "${CMAKE_SOURCE_DIR}/conanfile.txt"
            "BUILD;missing" # build missing, if specified
            "UPDATE" # update packages, if specified
            SETTINGS ${CI_AUTO_SETTINGS} ${CI_BUILD_TYPE} 
            GENERATOR cmake_find_package cmake_paths
        )

add_subdirectory(src)

