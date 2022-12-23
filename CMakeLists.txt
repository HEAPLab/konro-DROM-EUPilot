cmake_minimum_required (VERSION 3.0.0)
project (konro)

list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

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
