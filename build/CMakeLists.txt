cmake_minimum_required (VERSION 3.16)

project(Tank_War_Server C CXX)

include_directories(../include)
include_directories(../protobuf)
include_directories(../spdlog)
aux_source_directory(../src DIR_SRCS)
aux_source_directory(../protobuf DIR_PROTO)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

# set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(Tank_War_Server ${DIR_PROTO} ${DIR_SRCS})

target_link_libraries(Tank_War_Server ${Protobuf_LIBRARIES})

