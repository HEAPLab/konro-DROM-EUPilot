set(CMAKE_CXX_STANDARD 23)
file(GLOB platformcontrol_SOURCES "*.cpp")
file(GLOB platformcontrol_HEADERS "*.h")
file(GLOB platformcontrol_controllers_SOURCES "controllers/*.cpp")
file(GLOB platformcontrol_controllers_HEADERS "controllers/*.h")
file(GLOB utilities_SOURCES "utilities/*.cpp")
file(GLOB utilities_HEADERS "utilities/*.h")
file(GLOB cgroup_SOURCES "cgroup/*.cpp" "cgroup/controllers/*.cpp")
file(GLOB cgroup_HEADERS "cgroup/*.h" "cgroup/controllers/*.h")
file(GLOB drom_SOURCES "drom/*.cpp" "drom/controllers/*.cpp")
file(GLOB drom_HEADERS "drom/*.h" "drom/controllers/*.h")


add_library(platformcontrol
    ${platformcontrol_SOURCES} ${platformcontrol_HEADERS}
    ${platformcontrol_controllers_SOURCES} ${platformcontrol_controllers_HEADERS}
    ${cgroup_SOURCES} ${cgroup_HEADERS} ${drom_SOURCES} ${drom_HEADERS}
    ${utilities_SOURCES} ${utilities_HEADERS}
)

include_directories(${PROJECT_SOURCE_DIR}/rm/platformcontrol)
include_directories(${PROJECT_SOURCE_DIR}/rm/platformcontrol/utilities)
include_directories(${PROJECT_SOURCE_DIR}/rm/platformcontrol/cgroup)
include_directories(${PROJECT_SOURCE_DIR}/rm/platformcontrol/cgroup/controllers)
include_directories(${PROJECT_SOURCE_DIR}/rm/platformcontrol/drom)
include_directories(${PROJECT_SOURCE_DIR}/rm/platformcontrol/drom/controllers)


target_include_directories(platformcontrol INTERFACE .)
target_include_directories(platformcontrol INTERFACE cgroup/controllers)
target_include_directories(platformcontrol INTERFACE drom/controllers)
target_include_directories(platformcontrol INTERFACE utilities)

target_link_libraries(platformcontrol PRIVATE rmcommon ${LOG4CPP_LIBRARIES})

file(GLOB tpc_SOURCES "tpc/*.cpp")
file(GLOB tpc_HEADERS "tpc/*.h")

add_executable(tpc ${tpc_SOURCES} ${tpc_HEADERS})
target_link_libraries(tpc rmcommon platformcontrol)
