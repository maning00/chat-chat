cmake_minimum_required(VERSION 3.14)
project(server)

set(CMAKE_CXX_STANDARD 14)

add_executable(server main.cpp account.cpp account.hpp msgsvr.cpp Msgsvr.hpp protomsg.pb.cc protomsg.pb.h SharedQueue.h)
set(CMAKE_CXX_FLAGS "-lprotobuf")