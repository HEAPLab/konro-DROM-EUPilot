set(CMAKE_CXX_STANDARD 14)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(OpenCV REQUIRED opencv4)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB peopledetect_SOURCES "*.cpp")
file(GLOB peopledetect_HEADERS "*.h")

add_executable(peopledetect ${peopledetect_HEADERS} ${peopledetect_SOURCES})
target_link_libraries(peopledetect ${OpenCV_LIBS} konrolib)
