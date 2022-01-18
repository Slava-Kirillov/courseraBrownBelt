cmake_minimum_required(VERSION 3.19)
project(courseraRedBelt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )

add_executable(courseraRedBelt
        main.cpp
        parse.cpp
        parse.h
        search_server.cpp
        search_server.h
        iterator_range.h
        profile.h
        test_runner.h add_data.h sync.h)

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O2")