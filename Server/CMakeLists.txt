cmake_minimum_required (VERSION 3.16)

project(TankServer C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

# set(CMAKE_BUILD_TYPE "Debug")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lfmt -pthread")
ENDIF()


# 检查库文件是否存在（调试用）
message(STATUS "Checking libnet.so at: ${CMAKE_CURRENT_SOURCE_DIR}/bin/libnet.so")
message(STATUS "Checking libpublic.so at:  ${CMAKE_CURRENT_SOURCE_DIR}/bin/libpublic.so")

find_package(OpenSSL REQUIRED)

# 定义导入库（绝对路径 + 检查存在性）
find_library(NET_LIB net PATHS  ${CMAKE_CURRENT_SOURCE_DIR}/bin NO_DEFAULT_PATH)
find_library(PUBLIC_LIB public PATHS  ${CMAKE_CURRENT_SOURCE_DIR}/bin NO_DEFAULT_PATH)
find_library(FMT_LIB fmt PATHS /usr/lib /usr/local/lib)
find_library(URING_LIB uring PATHS /usr/lib /usr/local/lib)

if(NOT NET_LIB)
message(FATAL_ERROR "libnet.so not found in  ${CMAKE_CURRENT_SOURCE_DIR}/bin")
endif()
if(NOT PUBLIC_LIB)
message(FATAL_ERROR "libpublic.so not found in  ${CMAKE_CURRENT_SOURCE_DIR}/bin")
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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Net)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/publicShare)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/jwt-cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/public/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/GameStateCenter/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/UserFrontendService/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/GameService/include)


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/public/src DIR_PUBLIC_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/GameStateCenter/src DIR_GAMESTATECENTER_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/UserFrontendService/src DIR_USERFRONTENDSERVICE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/GameService/src DIR_GAMESERVICE_SRCS)


add_executable(GameStateCenter ${DIR_GAMESTATECENTER_SRCS} ${DIR_PUBLIC_SRCS})
target_link_libraries(GameStateCenter ${ALL_LIBS})

add_executable(UserFrontendService ${DIR_USERFRONTENDSERVICE_SRCS} ${DIR_PUBLIC_SRCS})
target_link_libraries(UserFrontendService ${ALL_LIBS})

add_executable(GameService ${DIR_GAMESERVICE_SRCS} ${DIR_PUBLIC_SRCS})
target_link_libraries(GameService ${ALL_LIBS})


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Debug 配置添加 _d 后缀
    set_target_properties(GameStateCenter PROPERTIES
        OUTPUT_NAME "GameStateCenter_d"  # 直接改名为 net_d
    )
    set_target_properties(UserFrontendService PROPERTIES
        OUTPUT_NAME "UserFrontendService_d"  # 直接改名为 net_d
    )
    set_target_properties(GameService PROPERTIES
        OUTPUT_NAME "GameService_d"  # 直接改名为 net_d
    )
endif()
