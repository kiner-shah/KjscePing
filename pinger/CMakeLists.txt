cmake_minimum_required(VERSION 3.15)
project(pinger VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)

set(PINGER_SOURCES
    utils.cpp
    WindowsDatagramSocket.cpp
    PosixDatagramSocket.cpp
    Socket.cpp
    ipv4_header.cpp
    icmp_header.cpp
    Pinger.cpp
    test.cpp)

find_package(Threads REQUIRED)
if (NOT Threads_FOUND)
    message(FATAL_ERROR "Threads not found")
endif()

add_executable(pinger ${PINGER_SOURCES})
target_link_libraries(pinger LINK_PUBLIC Threads::Threads)
if (MSVC OR WIN32)
    target_link_libraries(pinger LINK_PUBLIC Ws2_32)
endif()