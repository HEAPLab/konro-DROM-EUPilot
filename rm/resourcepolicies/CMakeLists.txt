set(CMAKE_CXX_STANDARD 14)
file(GLOB resourcepolicies_SOURCES "*.cpp")
file(GLOB resourcepolicies_HEADERS "*.h")
file(GLOB policies_SOURCES "policies/*.cpp")
file(GLOB policies_HEADERS "policies/*.h")
add_library(resourcepolicies
            ${resourcepolicies_HEADERS}
            ${resourcepolicies_SOURCES}
            ${policies_HEADERS}
            ${policies_SOURCES}
)

target_link_libraries(resourcepolicies PUBLIC rmcommon platformdescription platformcontrol)
target_include_directories(resourcepolicies INTERFACE .)
