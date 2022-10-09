set(CMAKE_CXX_STANDARD 14)
file(GLOB workloadmanager_SOURCES "*.cpp")
file(GLOB workloadmanager_HEADERS "*.h")

add_library(workloadmanager
    ${workloadmanager_SOURCES} ${workloadmanager_HEADERS}
)

include_directories(${PROJECT_SOURCE_DIR}/rm/include)

target_link_libraries(workloadmanager platformcontrol resourcepolicies)
target_include_directories(workloadmanager INTERFACE .)
