set (CMAKE_CXX_STANDARD 14)

add_subdirectory (platformcontrol)
add_subdirectory (workloadmanager)
add_subdirectory (resourcepolicies)

set(KONRO_SOURCES "src/main.cpp")

add_executable (konro ${KONRO_SOURCES})
target_compile_definitions(konro PUBLIC KONRO_RM)
target_link_libraries(konro PUBLIC konro-common platformcontrol workloadmanager)

configure_file(konro.ini.in konro.ini)
