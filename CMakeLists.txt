project(NOSA)
cmake_minimum_required(VERSION 2.8)
#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_BUILD_TYPE Debug)
set(BUILD_CURL_EXE false)
set(BUILD_CURL_TESTS false)
set(BUILD_PROJECT_STATIC false)
set(BUILD_CURL_STATIC true)

if(BUILD_PROJECT_STATIC)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin/static)
else()
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin/shared)
endif()

add_subdirectory(NOSA-shared)
add_subdirectory(NOSA-RemoteServer)
add_subdirectory(NOSA-Server)
if(WIN32)
    add_subdirectory(NOSA-PCClient)
endif()
