set(CMAKE_CXX_STANDARD 14)
file(GLOB platformdescription_SOURCES "*.cpp")
file(GLOB platformdescription_HEADERS "*.h")

add_library(platformdescription
    ${platformdescription_SOURCES} ${platformdescription_HEADERS}
    )

target_link_libraries(platformdescription PRIVATE rmcommon)
target_include_directories(platformdescription INTERFACE .)

