cmake_minimum_required(VERSION 3.25)
set(CMAKE_C_STANDARD 11)

project(itos_exp1 C)

find_package(Threads REQUIRED)

add_executable(e1q1 q1.c common.h)
add_executable(e1q2 q2.c common.h)
add_executable(e1q3 q3.c common.h)
add_executable(e1q4 q4.c common.h)
add_executable(e1q5rx q5rx.c common.h)
add_executable(e1q5tx q5tx.c common.h)
add_executable(e1q6rx q6rx.c common.h)
add_executable(e1q6tx q6tx.c common.h)

target_link_libraries(e1q6rx rt)
target_link_libraries(e1q6tx rt)