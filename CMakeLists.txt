cmake_minimum_required(VERSION 3.10)
project(helpers)

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Code Coverage
if(COVERAGE STREQUAL LLVM) 
    message(STATUS "Code coverage instrumentation enabled for LLVM")
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
endif()

include_directories(${CMAKE_SOURCE_DIR})

add_definitions(-DTESTS)

file(GLOB_RECURSE SRC  helpers/*.cpp helpers/*.h main.cpp)
add_executable(tests ${SRC})
