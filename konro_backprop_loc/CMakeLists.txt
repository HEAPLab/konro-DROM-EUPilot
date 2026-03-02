cmake_minimum_required(VERSION 3.1...3.26)

project( BackPropIntegration  VERSION 1.0.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(PATH_TO_KONROLIB "" CACHE PATH " path to konrolib" )
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH  ON)
# set(CMAKE_CXX_COMPILER clang++-18)

add_subdirectory(src)
target_compile_options( backprop PRIVATE  -Wpedantic -Wall -Wextra -Wconversion -Werror )

install(TARGETS backprop runner)



