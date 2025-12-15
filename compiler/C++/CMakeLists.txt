cmake_minimum_required(VERSION 3.20)
project(stratos)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(include)

# Gather source files (In a real project, you might list these explicitly to avoid re-globbing issues)
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/lexer/*.cpp"
    "src/parser/*.cpp"
    "src/sema/*.cpp"
    "src/codegen/*.cpp"
    "src/optimizer/*.cpp"
)

add_executable(stratos ${SOURCES})