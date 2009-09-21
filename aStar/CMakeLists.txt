cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(sokoban)
add_definitions(-fno-inline -Wall -g3)

include_directories(inc)
include_directories(inc/ex1)


set (SRCS
	main.cpp
	A_Star.cpp
	State_Base.cpp
	State.cpp)
	
add_executable(sokoban ${SRCS})