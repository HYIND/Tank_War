cmake_minimum_required (VERSION 3.16)

project(TankServer C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)


if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8)
endif()


# set(CMAKE_BUILD_TYPE "Debug")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -pthread")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PUBLIC_LIB ${CMAKE_CURRENT_SOURCE_DIR}/bin/libpublic_d.so)
    set(NET_LIB ${CMAKE_CURRENT_SOURCE_DIR}/bin/libnet_d.so)
else()
    set(PUBLIC_LIB ${CMAKE_CURRENT_SOURCE_DIR}/bin/libpublic.so)
    set(NET_LIB ${CMAKE_CURRENT_SOURCE_DIR}/bin/libnet.so)
endif()

set(BOX2D_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/libbox2d.a)

find_library(URING_LIB uring PATHS /usr/lib /usr/local/lib)

if(NOT URING_LIB)
    message(WARNING "uring library not found!")
endif()

elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BOX2D_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/box2d_d.lib)
    set(PUBLIC_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/public_d.lib)
    set(NET_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/net_d.lib)
else()
    set(BOX2D_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/box2d.lib)
    set(PUBLIC_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/public.lib)
    set(NET_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/net.lib)
endif()

ENDIF()

find_package(OpenSSL REQUIRED)

set(ALL_LIBS
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
include_directories(${OPENSSL_INCLUDE_DIR})


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/public/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/GameStateCenter/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/UserFrontendService/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/GameService/include)


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/public/src DIR_PUBLIC_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/GameStateCenter/src DIR_GAMESTATECENTER_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/UserFrontendService/src DIR_USERFRONTENDSERVICE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/GameService/src DIR_GAMESERVICE_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/GameService/src/ECS DIR_GAMESERVICE_ECS_SRCS)


add_executable(GameStateCenter ${DIR_GAMESTATECENTER_SRCS} ${DIR_PUBLIC_SRCS})
target_link_libraries(GameStateCenter ${ALL_LIBS})

add_executable(UserFrontendService ${DIR_USERFRONTENDSERVICE_SRCS} ${DIR_PUBLIC_SRCS})
target_link_libraries(UserFrontendService ${ALL_LIBS})

add_executable(GameService ${DIR_GAMESERVICE_SRCS} ${DIR_GAMESERVICE_ECS_SRCS} ${DIR_PUBLIC_SRCS})
target_link_libraries(GameService ${ALL_LIBS})
target_link_libraries(GameService ${BOX2D_LIB})


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
