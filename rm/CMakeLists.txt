set (CMAKE_CXX_STANDARD 14)

add_subdirectory (platformdescription)
add_subdirectory (librmcommon)
add_subdirectory (platformcontrol)
add_subdirectory (workloadmanager)
add_subdirectory (resourcepolicies)
add_subdirectory (platformmonitor)

file(GLOB konro_SOURCES "src/*.cpp")
file(GLOB konro_HEADERS "include/*.h")

include_directories(${PROJECT_SOURCE_DIR}/rm/include)

add_executable (konro ${konro_SOURCES} ${konro_HEADERS})
target_compile_definitions(konro PUBLIC KONRO_RM)
target_link_libraries(konro PUBLIC
    konro-common
    rmcommon
    platformcontrol
    workloadmanager
    resourcepolicies
    platformmonitor)

configure_file(konro.ini.in konro.ini)
