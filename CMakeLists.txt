cmake_minimum_required(VERSION 2.8.7)
project(uintr_test)

add_compile_options(-g -O3 -Wall -Werror -muintr)

add_library(uintr_lib STATIC uintr.cpp fd_common.cpp)
target_compile_options(uintr_lib PUBLIC -fPIC)

add_executable(server server.cpp)
add_executable(client client.cpp)

###########################################################
## COMMON
###########################################################

target_link_libraries(server uintr_lib)
target_link_libraries(client uintr_lib)
