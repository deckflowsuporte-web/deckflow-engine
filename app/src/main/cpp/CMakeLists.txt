cmake_minimum_required(VERSION 3.18.1)
add_library(native-lib SHARED native-lib.cpp)
target_link_libraries(native-lib android log)
