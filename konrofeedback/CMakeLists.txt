set(CMAKE_CXX_STANDARD 14)
file(GLOB konrofeedback_SOURCES "*.cpp")
file(GLOB konrofeedback_HEADERS "*.h")

add_library(konrofeedback ${konrofeedback_SOURCES} ${konrofeedback_HEADERS})

target_include_directories(konrofeedback INTERFACE .)
