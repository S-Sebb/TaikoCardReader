cmake_minimum_required(VERSION 3.24)
project(TaikoCardReader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(TaikoCardReader main.cpp)
