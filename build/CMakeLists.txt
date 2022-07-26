cmake_minimum_required (VERSION 3.16)

project(Tank_War_Server C CXX)

aux_source_directory(../source DIR_SRCS)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
add_executable(Tank_War_Server ${DIR_SRCS})

set(CMAKE_CXX_FLAGS "-pthread")


