set(CMAKE_CXX_STANDARD 14)
file(GLOB testsensors_SOURCES "*.cpp")
file(GLOB testsensors_HEADERS "*.h")

set(LMSENSORS_INCLUDE_DIRS "/usr/include/sensors")
set(LMSENSORS_LIBRARIES libsensors.so)

add_executable(testsensors ${testsensors_HEADERS} ${testsensors_SOURCES})
target_include_directories(testsensors PRIVATE ${LMSENSORS_INCLUDE_DIRS})
target_link_libraries(testsensors ${LMSENSORS_LIBRARIES})
