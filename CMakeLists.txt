cmake_minimum_required(VERSION 2.8)

project(balancer)
add_executable(${PROJECT_NAME} "main.cpp" "tcpsocket.cpp" "proxyserver.cpp" "upstreampool.cpp")

SET(CMAKE_CXX_FLAGS -pthread)
