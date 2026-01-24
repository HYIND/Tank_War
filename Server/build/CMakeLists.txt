cmake_minimum_required (VERSION 3.16)

project(TankServer C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lfmt -pthread")
ENDIF()


# 检查库文件是否存在（调试用）
message(STATUS "Checking libnet.so at: ${CMAKE_SOURCE_DIR}/../bin/libnet.so")
message(STATUS "Checking libpublic.so at: ${CMAKE_SOURCE_DIR}/../bin/libpublic.so")

find_package(OpenSSL REQUIRED)

# 定义导入库（绝对路径 + 检查存在性）
find_library(NET_LIB net PATHS ${CMAKE_SOURCE_DIR}/../bin NO_DEFAULT_PATH)
find_library(PUBLIC_LIB public PATHS ${CMAKE_SOURCE_DIR}/../bin NO_DEFAULT_PATH)
find_library(FMT_LIB fmt PATHS /usr/lib /usr/local/lib)
find_library(URING_LIB uring PATHS /usr/lib /usr/local/lib)

if(NOT NET_LIB)
message(FATAL_ERROR "libnet.so not found in ${CMAKE_SOURCE_DIR}/../bin")
endif()
if(NOT PUBLIC_LIB)
message(FATAL_ERROR "libpublic.so not found in ${CMAKE_SOURCE_DIR}/../bin")
endif()
if(NOT FMT_LIB)
message(FATAL_ERROR "fmt library not found!")
endif()
if(NOT URING_LIB)
message(FATAL_ERROR "uring library not found!")
endif()


set(ALL_LIBS
    ${FMT_LIB}
    ${NET_LIB}
    ${PUBLIC_LIB}
    ${OPENSSL_LIBRARIES}
)

if(URING_LIB)
    list(APPEND ALL_LIBS ${URING_LIB})
endif()

include_directories(../ThirdParty)
include_directories(../ThirdParty/Net)
include_directories(../ThirdParty/publicShare)
include_directories(../ThirdParty/jwt-cpp)
include_directories(../ThirdParty/spdlog)


include_directories(../source/public/include)
include_directories(../source/GameStateCenter/include)
include_directories(../source/UserFrontendService/include)
include_directories(../source/GameService/include)


aux_source_directory(../source/public/src DIR_PUBLIC_SRCS)
aux_source_directory(../source/GameStateCenter/src DIR_GAMESTATECENTER_SRCS)
aux_source_directory(../source/UserFrontendService/src DIR_USERFRONTENDSERVICE_SRCS)
aux_source_directory(../source/GameService/src DIR_GAMESERVICE_SRCS)


add_executable(GameStateCenter ${DIR_GAMESTATECENTER_SRCS} ${DIR_PUBLIC_SRCS})
target_link_libraries(GameStateCenter ${ALL_LIBS})

add_executable(UserFrontendService ${DIR_USERFRONTENDSERVICE_SRCS} ${DIR_PUBLIC_SRCS})
target_link_libraries(UserFrontendService ${ALL_LIBS})

add_executable(GameService ${DIR_GAMESERVICE_SRCS} ${DIR_PUBLIC_SRCS})
target_link_libraries(GameService ${ALL_LIBS})


