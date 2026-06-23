cmake_minimum_required(VERSION 3.10)
project(SubgraphMatching)

include_directories(
        ${PROJECT_SOURCE_DIR}/utility
        ${PROJECT_SOURCE_DIR}/
)
#
set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -g -Wall -march=native -pthread") #for runing

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Og -g -Wall -march=native -pthread")#for debug

#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(graph)
add_subdirectory(matching)
add_subdirectory(utility)


