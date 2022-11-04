set(CMAKE_CXX_STANDARD 14)
#find_package(PkgConfig REQUIRED)
#pkg_check_modules(LMSENSORS REQUIRED lmsensors)
set(LMSENSORS_INCLUDE_DIRS "/usr/include/sensors")
set(LMSENSORS_LIBRARIES libsensors.so)
file(GLOB platformmonitor_SOURCES "*.cpp")
file(GLOB platformmonitor_HEADERS "*.h")
add_library(platformmonitor
            ${platformmonitor_HEADERS}
            ${platformmonitor_SOURCES}
)

target_include_directories(platformmonitor INTERFACE .)
target_include_directories(platformmonitor PRIVATE ${LMSENSORS_INCLUDE_DIRS})
target_link_libraries(platformmonitor PUBLIC resourcepolicies  ${LMSENSORS_LIBRARIES})
