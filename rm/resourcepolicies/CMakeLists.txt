set(CMAKE_CXX_STANDARD 14)
file(GLOB resourcemanagement_SOURCES "*.cpp")
file(GLOB resourcemanagement_HEADERS "*.h")
add_library(resourcemanagement
            ${resourcemanagement_HEADERS}
            ${resourcemanagement_SOURCES}
)

target_include_directories(resourcemanagement INTERFACE .)
