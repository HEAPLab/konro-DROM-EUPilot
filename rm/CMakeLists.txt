
set (CMAKE_CXX_STANDARD 17)

add_subdirectory (platformcontrol)
add_subdirectory (workloadmanager)

set(KONRO_SOURCES "src/main.cpp")

add_executable (konro ${KONRO_SOURCES})
target_compile_definitions(konro PUBLIC KONRO_RM)
target_link_libraries(konro PUBLIC konro-common platformcontrol workloadmanager)

configure_file(konro.ini.in konro.ini)
