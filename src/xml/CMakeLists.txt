cmake_minimum_required(VERSION 3.6)
project(pfxml)

add_library(pfxml INTERFACE)
target_include_directories(pfxml INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
