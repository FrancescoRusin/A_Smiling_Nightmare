cmake_minimum_required(VERSION 3.28)
project(SS_game)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)
include(${CMAKE_MODULE_PATH}/SDL2-config.cmake)
include_directories(${CMAKE_SOURCE_DIR}/game)

file(GLOB SOURCES game/*.cpp)

add_executable(${PROJECT_NAME} Main.cpp;${SOURCES})

add_compile_options(-Wall)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options(-o3)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    include(PrintVariables.txt)
    add_compile_options(-g)
endif()