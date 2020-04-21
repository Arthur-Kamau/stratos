cmake_minimum_required(VERSION 3.15)
project(Stratos_Compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(Stratos_Compiler main.cpp file/file.cpp file/file.h path/path.cpp path/path.h sys/system.cpp sys/system.h)