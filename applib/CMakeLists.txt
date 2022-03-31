
set (CMAKE_CXX_STANDARD 17)

set(APPLIB_SOURCES "src/applib.cpp")

add_library (konroapp SHARED ${APPLIB_SOURCES})
target_compile_definitions(konroapp PUBLIC KONRO_APPLIB)
target_link_libraries(konroapp PUBLIC konro-common)


configure_file(applib.ini.in applib.ini)
