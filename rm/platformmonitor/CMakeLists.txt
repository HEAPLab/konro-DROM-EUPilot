set(CMAKE_CXX_STANDARD 14)
file(GLOB platformmonitor_SOURCES "*.cpp")
file(GLOB platformmonitor_HEADERS "*.h")
add_library(platformmonitor
            ${platformmonitor_HEADERS}
            ${platformmonitor_SOURCES}
)

target_include_directories(platformmonitor INTERFACE .)
target_link_libraries(platformmonitor PUBLIC resourcepolicies)
