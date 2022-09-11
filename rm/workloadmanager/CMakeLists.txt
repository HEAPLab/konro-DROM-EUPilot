set(CMAKE_CXX_STANDARD 14)
file(GLOB workloadmanager_SOURCES "*.cpp")
file(GLOB workloadmanager_HEADERS "*.h")

add_library(workloadmanager
    ${workloadmanager_SOURCES} ${workloadmanager_HEADERS}
    ${workloadmanager_controllers_SOURCES} ${workloadmanager_controllers_HEADERS}
)
target_include_directories(workloadmanager INTERFACE .)
