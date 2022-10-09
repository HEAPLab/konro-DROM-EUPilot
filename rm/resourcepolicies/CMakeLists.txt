set(CMAKE_CXX_STANDARD 14)
file(GLOB resourcepolicies_SOURCES "*.cpp")
file(GLOB resourcepolicies_HEADERS "*.h")
add_library(resourcepolicies
            ${resourcepolicies_HEADERS}
            ${resourcepolicies_SOURCES}
)

include_directories("../include")
target_link_libraries(resourcepolicies PRIVATE rmcommon)
target_include_directories(resourcepolicies INTERFACE .)

