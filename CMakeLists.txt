cmake_minimum_required(VERSION 3.14)
project(archiver_Fedorova_Alena_BSE185)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_executable(archiver_Fedorova_Alena_BSE185 main.cpp ShannonFano.cpp ShannonFano.h LZ77.cpp LZ77.h)