cmake_minimum_required(VERSION 3.19)
project(Linkaging)

set(CMAKE_CXX_STANDARD 11)

link_directories( /usr/lib/x86_64-linux-gnu/libpthread.so )
include_directories( /usr/lib/x86_64-linux-gnu/libpthread.a )

add_executable(Linkaging all.h main.cpp
        Server/Buffer.h Server/Buffer.cpp
        Server/HttpRequest.h Server/HttpRequest.cpp
        Server/ThreadPool.h Server/ThreadPool.cpp
        Server/Timer.h Server/Timer.cpp
        Server/Utils.h Server/Utils.cpp
        Server/Epoll.h Server/Epoll.cpp
        Server/HttpResponse.h   Server/HttpResponse.cpp
        Server/LkgServer.h Server/LkgServer.cpp
        Server/main.cpp
        )

target_link_libraries(Linkaging pthread)