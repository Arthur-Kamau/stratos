cmake_minimum_required(VERSION 3.17)
project(stratos_native)

include_directories("${PROJECT_SOURCE_DIR}")
set(CMAKE_CXX_STANDARD 20)

add_executable(stratos_native main.cpp Lexer.cpp Node.cpp NodeType.cpp Parser.cpp Token.cpp TokenType.cpp)