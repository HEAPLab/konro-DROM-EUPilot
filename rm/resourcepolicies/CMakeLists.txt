set(CMAKE_CXX_STANDARD 14)
file(GLOB resourcepolicies_SOURCES "*.cpp")
file(GLOB resourcepolicies_HEADERS "*.h")
add_library(resourcepolicies
            ${resourcepolicies_HEADERS}
            ${resourcepolicies_SOURCES}
)

include_directories("../include")
#include_directories("../platformcontrol")
target_include_directories(resourcepolicies INTERFACE .)

