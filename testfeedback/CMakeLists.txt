set(CMAKE_CXX_STANDARD 14)

file(GLOB testfeedback_SOURCES "*.cpp")
file(GLOB testfeedback_HEADERS "*.h")

add_executable(testfeedback ${testfeedback_HEADERS} ${testfeedback_SOURCES})
target_link_libraries(testfeedback PRIVATE konrofeedback)
