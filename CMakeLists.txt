cmake_minimum_required(VERSION 3.8)

project(MIPSMINIASSEMBLER)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(src)

add_subdirectory(src)
#add_subdirectory(lib/googletest)



