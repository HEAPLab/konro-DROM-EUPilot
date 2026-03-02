file(GLOB librmcommon_SOURCES "*.cpp")
file(GLOB librmcommon_HEADERS "*.h"  "events/*.h")

find_package(Threads REQUIRED)
add_library(rmcommon ${librmcommon_SOURCES} ${librmcommon_HEADERS})

target_include_directories(rmcommon INTERFACE .)
target_include_directories(rmcommon INTERFACE events)
target_link_libraries(rmcommon Threads::Threads)
