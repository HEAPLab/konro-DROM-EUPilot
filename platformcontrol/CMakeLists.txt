set(CMAKE_CXX_STANDARD 14)
file(GLOB platformcontrol_SOURCES "*.cpp")
file(GLOB platformcontrol_HEADERS "*.h")
file(GLOB platformcontrol_controllers_SOURCES "controllers/*.cpp")
file(GLOB platformcontrol_controllers_HEADERS "controllers/*.h")

add_library(platformcontrol
    ${platformcontrol_SOURCES} ${platformcontrol_HEADERS}
    ${platformcontrol_controllers_SOURCES} ${platformcontrol_controllers_HEADERS}
)
target_include_directories(platformcontrol INTERFACE .)
target_include_directories(platformcontrol INTERFACE controllers)

file(GLOB tpc_SOURCES "tpc/*.cpp")
file(GLOB tpc_HEADERS "tpc/*.h")

add_executable(tpc ${tpc_SOURCES} ${tpc_HEADERS})
target_link_libraries(tpc platformcontrol)
