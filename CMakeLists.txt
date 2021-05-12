cmake_minimum_required(VERSION 3.19)
project(Linkaging)

set(CMAKE_CXX_STANDARD 11)

add_executable(Linkaging all.h main.cpp
        Server/Buffer.h Server/Buffer.cpp
        Server/HttpRequest.h Server/HttpRequest.cpp
        Server/ThreadPool.h Server/ThreadPool.cpp
        Server/Timer.h Server/Timer.cpp
        Server/Utils.h Server/Utils.cpp
        )