add_executable(backprop backprop_kernel.cpp backprop.cpp backprop.h facetrain.cpp imagenet.cpp konrolib.h)
target_link_libraries(backprop PRIVATE "${PATH_TO_KONROLIB}")
target_link_libraries(backprop PRIVATE  "/usr/local/lib/libdlb.so")
target_include_directories(backprop PRIVATE "/usr/lib/llvm-14/include")
find_package(OpenMP 5 MODULE  )
# if(OpenMP_VERSION VERSION_LESS "5.0" )
#    message(FATAL_ERROR "OpenMP version not supported  ${OpenMP_VERSION}  < 5.0 ") 
# endif()

target_link_libraries(backprop PUBLIC OpenMP::OpenMP_CXX)


add_executable(runner exec.cpp)
target_link_libraries(runner PRIVATE "${PATH_TO_KONROLIB}")
target_link_libraries(runner PRIVATE  "/usr/local/lib/libdlb.so")
