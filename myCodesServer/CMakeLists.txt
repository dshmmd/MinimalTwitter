cmake_minimum_required(VERSION 3.17)
project(myCodesServer C)

set(CMAKE_C_STANDARD 99)

add_executable(myCodesServer main.c cJSON.c)

target_link_libraries(myCodesServer ws2_32)