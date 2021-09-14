cmake_minimum_required(VERSION 3.0.0)
project(tcpip VERSION 0.1.0)


include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(server computersystem/server.cpp)
add_executable(client computersystem/client.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
