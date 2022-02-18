cmake_minimum_required(VERSION 3.19)
project(courseraBrownBelt)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )

add_executable(courseraRedBelt
        collide.cpp
        profile.h
        test_runner.h
        geo2d.h
        geo2d.cpp
        game_object.h
        collide.cpp)

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O2")