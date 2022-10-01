cmake_minimum_required(VERSION 3.23)
project(backup_tool)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(backup_tool
        backup.cpp)
