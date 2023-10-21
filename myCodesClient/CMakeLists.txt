cmake_minimum_required(VERSION 3.17)
project(myCodesClient C)

set(CMAKE_C_STANDARD 99)

add_executable(myCodesClient main.c cJSON.c)

target_link_libraries(myCodesClient ws2_32)