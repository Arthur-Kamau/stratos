cmake_minimum_required(VERSION 3.20)
project(stratos)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(include)

# Find SDL2 packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# Gather source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/lexer/*.cpp"
    "src/parser/*.cpp"
    "src/sema/*.cpp"
    "src/codegen/*.cpp"
    "src/optimizer/*.cpp"
    "src/runtime/*.cpp"
    "src/runtime/gui/*.cpp"
    "src/stui/*.cpp"
    "src/config/*.cpp"
    "src/devtools/*.cpp"
    "src/doc/*.cpp"
)

add_executable(stratos ${SOURCES})

target_link_libraries(stratos
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    pthread dl ssl crypto pq mysqlclient hiredis
)
