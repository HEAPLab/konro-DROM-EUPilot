# To enable timing:  cmake -DTIMING=ON

cmake_minimum_required (VERSION 3.0.0)
project (konro)

list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

option(TIMING "Enable low level timing" OFF)

if (TIMING)
    # enable low level timing of (some parts of the) code
    add_definitions(-DTIMING)
endif()

add_subdirectory (common)
add_subdirectory (lib)
add_subdirectory (rm)
add_subdirectory (konrolib)

add_subdirectory (testplatformcontrol)
add_subdirectory (testthreadsafequeue)
add_subdirectory (testsensors)
add_subdirectory (testhwloc)
add_subdirectory (testkonrolib)
add_subdirectory (peopledetect)
