cmake_minimum_required(VERSION 2.8)

project(technika)
add_executable(technika play.cpp TechnikaSong.cpp TechnikaSong.hpp SoundPool.cpp SoundPool.hpp)

# required libraries
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
target_link_libraries(technika ${SDL2_LIBRARIES})

pkg_search_module(SNDFILE REQUIRED sndfile)
target_link_libraries(technika ${SNDFILE_LIBRARIES})

# OpenAL
if (APPLE)
    find_library(OPENAL_LIBRARY OpenAL)
    target_link_libraries(technika ${OPENAL_LIBRARY})
else (APPLE)
    find_library(OPENAL_LIBRARY
        NAMES OpenAL al openal
        PATHS
        /usr
        /usr/local
        /opt
        /opt/local)
endif (APPLE)
find_path(OPENAL_INCLUDE_DIR al.h)

# C++11 
if (UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-unknown-pragmas -std=c++11")
endif (UNIX)

include_directories(${SDL2_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR} ${SNDFILE_INCLUDE_DIRS})

set(CMAKE_BUILD_TYPE Debug)
