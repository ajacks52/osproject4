cmake_minimum_required(VERSION 3.3)
project(lab4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu99 -g")

set(SOURCE_FILES elevator_skeleton.c elevator.h elevator_part_1.c finesleep.c dllist.h dllist.c fields.c jval.c jrb.c)

add_executable(elevator_part_1 ${SOURCE_FILES})
