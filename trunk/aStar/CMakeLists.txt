cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(sokoban)
add_definitions(-fno-inline -Wall -g3)

include_directories(.)
include_directories(inc)
include_directories(inc/ex1)
include_directories(inc/ex2)


set (SRCS_ASTAR_DEMO
	main.cpp
	A_Star.cpp
	State.cpp)
	
	add_executable(aStarDemo ${SRCS_ASTAR_DEMO})
